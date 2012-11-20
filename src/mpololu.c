/**
 * @file   mpololu.c
 * @Author kls (gbkletsko@gmail.com)
 * @date   November, 2012
 * @brief  Maestro Pololu communication library.
 *
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include "mpololu.h"


#define ANSWER_GET_POSITION_SIZE 0x02
#define ANSWER_GET_ERRORS_SIZE 0x02
#define ANSWER_IS_MOVING_SIZE 0x01
#define ANSWER_IS_STOPPED_SIZE 0x01

#define POLOLU_PROTO_ON 0xAA
#define MINISSC_PROTO_ON 0xFF

#define COMPACT_SET_TARGET 0x84
#define POLOLU_SET_TARGET 0x04

#define COMPACT_SET_MULTARGET 0x9F
#define POLOLU_SET_MULTARGET 0x1F

#define COMPACT_SET_SPEED 0x87
#define POLOLU_SET_SPEED 0x07

#define COMPACT_SET_ACCELERATION 0x89
#define POLOLU_SET_ACCELERATION 0x09

#define COMPACT_SET_PWM 0x8A
#define POLOLU_SET_PWM 0x0A

#define COMPACT_GET_POSITION 0x90
#define POLOLU_GET_POSITION 0x10

#define COMPACT_GET_POSITION 0x90
#define POLOLU_GET_POSITION 0x10

#define COMPACT_GET_MOVING_STATE 0x93
#define POLOLU_GET_MOVING_STATE 0x13

#define COMPACT_GET_ERRORS 0xA1
#define POLOLU_GET_ERRORS 0x21

#define COMPACT_GO_HOME 0xA2
#define POLOLU_GO_HOME 0x22

#define COMPACT_STOP_SCRIPT 0xA4
#define POLOLU_STOP_SCRIPT 0x24

#define COMPACT_RESTART_SCRIPT 0xA7
#define POLOLU_RESTART_SCRIPT 0x27

#define COMPACT_RESTART_SCRIPT_PAR 0xA8
#define POLOLU_RESTART_SCRIPT_PAR 0x28

#define COMPACT_GET_SCRIPT_STATUS 0xAE
#define POLOLU_GET_SCRIPT_STATUS 0x2E



static void dump_cmd(uint8_t* cmd, int32_t len)
{
	int i = 0;
	printf("write command: \n");
	for (i = 0; i < len; i++) {
		printf("\t0x%X\n",cmd[i]);
	}
}
/**
 * @brief Open serial interface
 *  
 *  @retval File desriptor of opened interface, -1 - if error occured
 */
int32_t maestro_open(const char* device)
{
	struct termios options;	
	int fd;
	
	if (device == NULL) {
		fprintf(stderr, "No device name presented\n");
		return -1;
	}

	fd = open(device, O_RDWR | O_NOCTTY);
	
	if (fd == -1) {
		perror(device);
		return -1;
	}

	tcgetattr(fd, &options);
	options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	options.c_oflag &= ~(ONLCR | OCRNL);
	/* cfsetispeed(&options, B9600); */
	/* cfsetospeed(&options, B9600); */
	tcsetattr(fd, TCSANOW, &options);
	
   
	return fd;
}


/**
 * @brief Close serial interface
 *  
 */
int32_t maestro_close(int32_t fd)
{
	if (close(fd)) {
		perror("error closing");
		return -1;
	}
	return 0;
}


/**
 * @brief Maestro set target (Pololu protocol)
 */
int32_t maestro_pololu_set_target(int32_t fd, uint8_t device, uint8_t channel, uint16_t target)
{
	uint8_t command[6];
	int wr = 0;
	
	command[0] = POLOLU_PROTO_ON;
	command[1] = device;
	command[2] = POLOLU_SET_TARGET;
	command[3] = channel;
	command[4] = target & 0x7F;
	command[5] = (target >> 7) & 0x7F;
	
	dump_cmd(command, sizeof (command));

	if ((wr = write(fd, command, sizeof(command))) != sizeof(command)) {
		perror("error writing");
		return -1;
	}
	
	return 0;
}

/**
 * @brief Maestro set target (Compact protocol)
 */
int32_t maestro_compact_set_target(int32_t fd, uint8_t channel, uint16_t target)
{  
	uint8_t command[4];
	int wr = 0;
		
	
	command[0] = COMPACT_SET_TARGET;
	command[1] = channel;
	command[2] = target & 0x7F;
	command[3] = (target >> 7) & 0x7F;   
		
	dump_cmd(command, sizeof (command));

	if ((wr = write(fd, command, sizeof(command))) != sizeof(command)) {
		perror("error writing");
		return -1;
	}
	
	return 0;
}


/**
 * @brief Maestro set target (MiniSSC protocol)
 */
int32_t maestro_minissc_set_target(int32_t fd, uint8_t channel, uint8_t target)
{
	uint8_t command[3];
	int wr = 0;
	
	command[0] = MINISSC_PROTO_ON;
	command[1] = channel;
	command[2] = target;	
	dump_cmd(command, sizeof (command));	
	if ((wr = write(fd, command, sizeof(command))) != sizeof(command)) {
		perror("error writing");
		return -1;
	}
	
	return 0;
}

/**
 * @brief Maestro set multiple target (Pololu protocol)
 * 
 */
int32_t maestro_pololu_set_multiple_target(int32_t fd, uint8_t device, uint8_t targets_num, uint8_t first_channel, uint16_t* targets_p)
{
	uint8_t *command;	
	int wr = 0;
	int i;

	size_t cmd_sz = 5 /* command header*/ + 2 * targets_num;
	
	if ((targets_p == NULL) & (targets_num != 0)) {
		fprintf(stderr, "NULL pointer\n");
		return -1;
	}

	command = (uint8_t*) calloc(cmd_sz, sizeof(uint8_t));
	command[0] = POLOLU_PROTO_ON;
	command[1] = device;
	command[2] = POLOLU_SET_MULTARGET;
	command[3] = targets_num;
	command[4] = first_channel;
	
	for (i = 0; i < targets_num; i++) {
		command[5 + 2 * i] = targets_p[i] & 0x7F;
		command[6 + 2 * i] = (targets_p[i] >> 7) & 0x7F;
	}
	
	dump_cmd(command, cmd_sz);
	if ((wr = write(fd, command, cmd_sz)) != cmd_sz) {
		perror("error writing");
		return -1;
	}
	
	return 0;
}

/**
 * @brief Maestro set multiple target (Compact protocol)
 * 
 * For sending target bytes use maestro_send_target() function repeatedly.
 */
int32_t maestro_compact_set_multiple_target(int32_t fd, uint8_t targets_num, uint8_t first_channel, uint16_t* targets_p)
{
	uint8_t *command;	
	int wr = 0;
	int i;

	size_t cmd_sz = 3 /* command header*/ + 2 * targets_num;
	
	if ((targets_p == NULL) & (targets_num != 0)) {
		fprintf(stderr, "NULL pointer\n");
		return -1;
	}

	command = (uint8_t*) calloc(cmd_sz, sizeof(uint8_t));
	
	command[0] = COMPACT_SET_MULTARGET;
	command[1] = targets_num;
	command[2] = first_channel;

	for (i = 0; i < targets_num; i++) {
		command[3 + 2 * i] = targets_p[i] & 0x7F;
		command[4 + 2 * i] = (targets_p[i] >> 7) & 0x7F;
	}
	
	dump_cmd(command, cmd_sz);
	if ((wr = write(fd, command, cmd_sz)) != cmd_sz) {
		perror("error writing");
		return -1;
	}
	
	return 0;
}


/**
 *  @brief Set speed (Pololu protocol)
 */
int32_t maestro_pololu_set_speed(int32_t fd, uint8_t device, uint8_t channel, uint16_t speed)
{
	uint8_t command[6];
	int wr = 0;
	
	command[0] = POLOLU_PROTO_ON;
	command[1] = device;
	command[2] = POLOLU_SET_SPEED;
	command[3] = channel;
	command[4] = speed & 0x7F;
	command[5] = (speed >> 7) & 0x7F;   
	dump_cmd(command, sizeof (command));
	if ((wr = write(fd, command, sizeof(command))) != sizeof(command)) {
		perror("error writing");
		return -1;
	}
	
	return 0;
}

/**
 *  @brief Set speed (Compact protocol)
 */
int32_t maestro_compact_set_speed(int32_t fd, uint8_t channel, uint16_t speed)
{	
	uint8_t command[4];
	int wr = 0;
	
	command[0] = COMPACT_SET_SPEED;
	command[1] = channel;
	command[2] = speed & 0x7F;
	command[3] = (speed >> 7) & 0x7F;	
	dump_cmd(command, sizeof (command));
	if ((wr = write(fd, command, sizeof(command))) != sizeof(command)) {
		perror("error writing");
		return -1;
	}
	
	return 0;
}

/**
 *  @brief Set acceleration (Pololu protocol)
 */
int32_t maestro_pololu_set_acceleration(int32_t fd, uint8_t device, uint8_t channel, uint16_t acceleration)
{
	uint8_t command[6];
	int wr = 0;
		
	command[0] = POLOLU_PROTO_ON;
	command[1] = device;
	command[2] = POLOLU_SET_ACCELERATION;
	command[3] = channel;
	command[4] = acceleration & 0x7F;
	command[5] = (acceleration >> 7) & 0x7F;   
	dump_cmd(command, sizeof (command));
	if ((wr = write(fd, command, sizeof(command))) != sizeof(command)) {
		perror("error writing");
		return -1;
	}
	
	return 0;
}

/**
 *  @brief Set acceleration (Compact protocol)
 */
int32_t maestro_compact_set_acceleration(int32_t fd, uint8_t channel, uint16_t acceleration)
{
	uint8_t command[4];
	int wr = 0;
	
	command[0] = COMPACT_SET_ACCELERATION;
	command[1] = channel;
	command[2] = acceleration & 0x7F;
	command[3] = (acceleration >> 7) & 0x7F;
	dump_cmd(command, sizeof (command));		
	if ((wr = write(fd, command, sizeof(command))) != sizeof(command)) {
		perror("error writing");
		return -1;
	}
	
	return 0;
}

/**
 *  @brief Set PWM (Pololu protocol)
 */
int32_t maestro_pololu_set_pwm(int32_t fd, uint8_t device, uint16_t on_time, uint16_t period)
{
	uint8_t command[7];
	int wr = 0;
		
	command[0] = POLOLU_PROTO_ON;
	command[1] = device;
	command[2] = POLOLU_SET_PWM;
	command[3] = on_time & 0x7F;
	command[4] = (on_time >> 7) & 0x7F;
	command[5] = period & 0x7F;
	command[6] = (period >> 7) & 0x7F;   
	dump_cmd(command, sizeof (command));
	if ((wr = write(fd, command, sizeof(command))) != sizeof(command)) {
		perror("error writing");
		return -1;
	}
	
	return 0;
}

/**
 *  @brief Set PWM (Compact protocol)
 */
int32_t maestro_compact_set_pwm(int32_t fd, uint16_t on_time, uint16_t period)
{
	uint8_t command[5];	
	int wr = 0;
	
	command[0] = COMPACT_SET_PWM;
	command[1] = on_time & 0x7F;
	command[2] = (on_time >> 7) & 0x7F;
	command[3] = period & 0x7F;
	command[4] = (period >> 7) & 0x7F;
	dump_cmd(command, sizeof (command));
	if ((wr = write(fd, command, sizeof(command))) != sizeof(command)) {
		perror("error writing");
		return -1;
	}
	
	return 0;
}


static int32_t maestro_get_small_answer(int32_t fd, 
                                        uint8_t* cmd, 
                                        size_t len, 
                                        struct timeval* timeout,
                                        size_t ans_len) 
{
	fd_set set;
	int wr = 0;
	int rd = 0;   
	int rv;
	int32_t res = 0;
	int i = 0;
	uint8_t* answer = (uint8_t*) calloc(1, ans_len);
	
	if (!answer) {
		perror("calloc()");
		return -1;
	}

	if (ans_len > sizeof (int32_t)) {
		fprintf(stderr, "ans_len > sizeof int32_t\n"); 
		free(answer);
		return -1;
	}

	if ((wr = write(fd, cmd, len)) != len) {
		perror("error writing");
		free(answer);
		return -1;
	}
	
	FD_ZERO(&set); 
	FD_SET(fd, &set); 
	
	rv = select(fd + 1, &set, NULL, NULL, timeout);
	
	if (rv == -1)
		perror("select"); /* an error accured */
	else if (rv == 0)
		fprintf(stderr, "timeout get_postion()\n"); /* a timeout occured */
	else {
		if ((rd = read(fd, answer, ans_len)) != ans_len) {
			fprintf(stderr, "incorrect answer size %d\n", rd);
		} 				
		
		for (i = 0; i < ans_len; i++) {
			res += answer[i] << (8 * i);
		}
	
		free(answer);
		return res;
	}
	
	free(answer);
	return -1;
}



/**
 *  @brief Get position (Pololu protocol)
 *  
 *  @retval 16 bit position value
 */
int32_t maestro_pololu_get_position(int32_t fd, uint8_t device, uint8_t channel, struct timeval* timeout)
{
	int32_t res = 0;
	uint8_t command[4];
	
	command[0] = POLOLU_PROTO_ON;
	command[1] = device;
	command[2] = POLOLU_GET_POSITION;
	command[3] = channel;
	dump_cmd(command, sizeof (command));
	res = maestro_get_small_answer(fd, &command[0], sizeof command, timeout, ANSWER_GET_POSITION_SIZE);

	return res;
}


/**
 *  @brief Get position (Compact protocol)
 *  
 *  @retval 16 bit position value
 */
int32_t maestro_compact_get_position(int32_t fd, uint8_t channel, struct timeval* timeout)
{
	int32_t res = 0;
	uint8_t command[2];

	command[0] = COMPACT_GET_POSITION;
	command[1] = channel;
	dump_cmd(command, sizeof (command));
	res = maestro_get_small_answer(fd, &command[0], sizeof command, timeout, ANSWER_GET_POSITION_SIZE);
	
	return res;
}


/**
 *  @brief Get moving state (Pololu protocol)
 *
 *  @retval 0 - if no servos are moving, 1 - otherwise.
 */
int32_t maestro_pololu_is_moving(int32_t fd, uint8_t device, struct timeval* timeout)
{
	int32_t res = 0;
	uint8_t command[3]; 
	
	command[0] = POLOLU_PROTO_ON;
	command[1] = device;
	command[2] = POLOLU_GET_MOVING_STATE;
	dump_cmd(command, sizeof (command));
	res = maestro_get_small_answer(fd, &command[0], sizeof command, timeout, ANSWER_IS_MOVING_SIZE);
	
	return res;
}

/**
 *  @brief Get moving state (Compact protocol)
 *
 *  @retval 0 - if no servos are moving, 1 - otherwise.
 */
int32_t maestro_compact_is_moving(int32_t fd, struct timeval* timeout)
{
	int32_t res = 0;
	uint8_t command[1] = {COMPACT_GET_MOVING_STATE};
	dump_cmd(command, sizeof (command));
	res = maestro_get_small_answer(fd, &command[0], sizeof command, timeout, ANSWER_IS_MOVING_SIZE);
	
	return res;
}

/**
 *  @brief Get errors (Pololu protocol)
 * 
 *  @retval 16 bit error value
 */
int32_t maestro_pololu_get_errors(int32_t fd, uint8_t device, struct timeval* timeout)
{
	int32_t res = 0;
	uint8_t command[3];
	
	command[0] = POLOLU_PROTO_ON;
	command[1] = device;
	command[2] = POLOLU_GET_ERRORS;
	dump_cmd(command, sizeof (command));
	res = maestro_get_small_answer(fd, &command[0], sizeof command, timeout, ANSWER_GET_ERRORS_SIZE);
	
	return res;
}

/**
 *  @brief Get errors (Compact protocol)
 * 
 *  @retval 16 bit error value
 */
int32_t maestro_compact_get_errors(int32_t fd, struct timeval* timeout)
{
	int32_t res = 0;
	uint8_t command[1] = {COMPACT_GET_ERRORS};
	dump_cmd(command, sizeof (command));
	res = maestro_get_small_answer(fd, &command[0], sizeof command, timeout, ANSWER_GET_ERRORS_SIZE);
	
	return res;
}

/**
 *  @brief Go home (Pololu protocol)
 */
int32_t maestro_pololu_go_home(int32_t fd, uint8_t device)
{	
	uint8_t command[3];
	
	int wr = 0;
	
	command[0] = POLOLU_PROTO_ON;
	command[1] = device;
	command[2] = POLOLU_GO_HOME;
	dump_cmd(command, sizeof (command));
	if ((wr = write(fd, command, sizeof(command))) != sizeof(command)) {
		perror("error writing");
		return -1;
	}
	
	return 0;
}

/**
 *  @brief Go home (Compact protocol)
 */
int32_t maestro_compact_go_home(int32_t fd)
{
	uint8_t command[1] = {COMPACT_GO_HOME};
	
	int wr = 0;
	dump_cmd(command, sizeof (command));
	if ((wr = write(fd, command, sizeof(command))) != sizeof(command)) {
		perror("error writing");
		return -1;
	}
  
	return 0;
}

/** Scripts API */
/**
 *  @brief Stop script (Pololu protocol)
 */
int32_t maestro_pololu_stop_script(int32_t fd, uint8_t device)
{
	uint8_t command[3];		
	int wr = 0;
	
	command[0] = POLOLU_PROTO_ON;
	command[1] = device;
	command[2] = POLOLU_STOP_SCRIPT;
	dump_cmd(command, sizeof (command));
	if ((wr = write(fd, command, sizeof(command))) != sizeof(command)) {
		perror("error writing");
		return -1;
	}
	
	return 0;
}

/**
 *  @brief Stop script (Compact protocol)
 */
int32_t maestro_compact_stop_script(int32_t fd)
{
	uint8_t command[] = {COMPACT_STOP_SCRIPT};
	
	int wr = 0;
	dump_cmd(command, sizeof (command));
	if ((wr = write(fd, command, sizeof(command))) != sizeof(command)) {
		perror("error writing");
		return -1;
	}
	
	return 0;
}

/**
 *  @brief Restart script at subroutine (Pololu protocol)
 */
int32_t maestro_pololu_restart_script(int32_t fd, uint8_t device, uint8_t subroutine_number)
{
	uint8_t command[4];
	int wr = 0;
	
	command[0] = POLOLU_PROTO_ON;
	command[1] = device;
	command[2] = POLOLU_RESTART_SCRIPT;
	command[3] = subroutine_number;
	dump_cmd(command, sizeof (command));
	if ((wr = write(fd, command, sizeof(command))) != sizeof(command)) {
		perror("error writing");
		return -1;
	}
	
	return 0;	
}

/**
 *  @brief Restart script at subroutine (Compact protocol)
 */
int32_t maestro_compact_restart_script(int32_t fd, uint8_t subroutine_number)
{
	uint8_t command[2];
	
	int wr = 0;
	
	command[0] = COMPACT_RESTART_SCRIPT;
	command[1] = subroutine_number;
	dump_cmd(command, sizeof (command));
	if ((wr = write(fd, command, sizeof(command))) != sizeof(command)) {
		perror("error writing");
		return -1;
	}
	
	return 0;		
}

/**
 *  @brief Restart script at subroutine with parameter (Pololu protocol)
 */
int32_t maestro_pololu_restart_script_par(int32_t fd, 
                                          uint8_t device, 
                                          uint8_t subroutine_number, 
                                          uint16_t parameter)
{
	uint8_t command[6];
	int wr = 0;
		
	command[0] = POLOLU_PROTO_ON;
	command[1] = device;
	command[2] = POLOLU_RESTART_SCRIPT_PAR;
	command[3] = subroutine_number;
	command[4] = parameter & 0x7F;
	command[5] = (parameter >> 7) & 0x7F;	
	dump_cmd(command, sizeof (command));
	if ((wr = write(fd, command, sizeof(command))) != sizeof(command)) {
		perror("error writing");
		return -1;
	}
	
	return 0;	
}

/**
 *  @brief Restart script at subroutine with parameter (Compact protocol)
 */
int32_t maestro_compact_restart_script_par(int32_t fd, 
                                           uint8_t subroutine_number, 
                                           uint16_t parameter)
{
	uint8_t command[4];	
	int wr = 0;

	command[0] = COMPACT_RESTART_SCRIPT_PAR;
	command[1] = subroutine_number;
	command[2] = parameter & 0x7F;
	command[3] = (parameter >> 7) & 0x7F;
	dump_cmd(command, sizeof (command));
	if ((wr = write(fd, command, sizeof(command))) != sizeof(command)) {
		perror("error writing");
		return -1;
	}
	
	return 0;		
}

/**
 *  @brief Get script status (Pololu protocol)
 *  @retval 0 - script is running, 1 - script is stopped
 */
int32_t maestro_pololu_is_stopped(int32_t fd, uint8_t device, struct timeval* timeout)
{
	int32_t res = 0;
	uint8_t command[3];
	
	command[0] = POLOLU_PROTO_ON;
	command[1] = device;
	command[2] = POLOLU_GET_SCRIPT_STATUS;
	dump_cmd(command, sizeof (command));
	res = maestro_get_small_answer(fd, &command[0], sizeof command, timeout, ANSWER_IS_STOPPED_SIZE);
	
	return res;

}

/**
 *  @brief Get script status (Compact protocol)
 *  @retval 0 - script is running, 1 - script is stopped
 */
int32_t maestro_compact_is_stopped(int32_t fd, struct timeval* timeout)
{
	int32_t res = 0;
	uint8_t command[1] = {COMPACT_GET_SCRIPT_STATUS};
	dump_cmd(command, sizeof (command));
	res = maestro_get_small_answer(fd, &command[0], sizeof command, timeout, ANSWER_IS_STOPPED_SIZE);
	
	return res;   
}
