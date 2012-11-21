#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>
#include "mpololu.h" /* Maestro Pololu Lib */

#define LINE_MAX (255)

int32_t device = -1;
int32_t channel = -1;

int32_t target = -1;

int32_t speed = -1;
int32_t acceleration = -1;

int32_t timeout = -1;

int32_t pwm_ontime = -1;
int32_t pwm_period = -1;

int get_position = 0;
int get_errors = 0;
int ssc = 0;

int go_home = 0;
int stop = 0;

int restart = -1;
int32_t parameter = -1;

int is_stop = 0;
int is_moving = 0;

int mult_num = -1;
int mult_first = -1;

char *file = NULL;

char *device_file = "/dev/ttyACM0";

struct timeval tv;


static void pr_errors (uint16_t err_code) 
{
	if (err_code & POLOLU_ERR_SIG) {
		fprintf(stdout, "Serial Signal Error\n");
	}

	if (err_code & POLOLU_ERR_OVR) {
		fprintf(stdout, "Serial Overrun Error\n");
	}

	if (err_code & POLOLU_ERR_RX) {
		fprintf(stdout, "Serial RX buffer full\n");
	}

	if (err_code & POLOLU_ERR_CRC) {
		fprintf(stdout, "Serial CRC error\n");
	}

	if (err_code & POLOLU_ERR_PROTO) {
		fprintf(stdout, "Serial protocol error \n");
	}

	if (err_code & POLOLU_ERR_TIMEOUT) {
		fprintf(stdout, "Serial timeout error \n");
	}

	if (err_code & POLOLU_ERR_STACK) {
		fprintf(stdout, "Serial stack error \n");
	}

	if (err_code & POLOLU_ERR_CALLSTACK) {
		fprintf(stdout, "Serial call stack error \n");
	}

	if (err_code & POLOLU_ERR_COUNTER) {
		fprintf(stdout, "Serial program counter error \n");
	}

}

static int32_t get_targets_list(char* file_name, uint16_t **targets_p)
{
	FILE* fp;
	char line[LINE_MAX];   
	int32_t sz = 0;

	fprintf(stdout, "Processing file %s...\n", file);
		
	fp = fopen(file, "r");
		
	if (fp == NULL) {
		perror("failed to open file");
		return -1;			
	}
		
	while (fgets(line, LINE_MAX, fp) != NULL) {
		int target = atoi(line);
		
		fprintf(stdout, "Target %u...\n", (uint16_t) target);
		
		sz++;
		
		*targets_p = (uint16_t *) realloc(*targets_p, sz * sizeof(uint16_t));

		(*targets_p)[sz - 1] = (uint16_t) target;
	}		

	return sz;
}

static void exec_cmds_compact (int32_t fd) 
{	
	/** Options */
	if (speed != -1) {
		if (maestro_compact_set_speed(fd, (channel == -1) ? 0 : (uint8_t)channel, (uint16_t)speed) < 0) {
			fprintf(stderr, "Failed to set speed");
			return;
		}
	}

	if (acceleration != -1) {
		if (maestro_compact_set_acceleration(fd, (channel == -1) ? 0 : (uint8_t)channel, (uint16_t)acceleration) < 0) {
			fprintf(stderr, "Failed to set acceleration");
			return;
		}
	}
	
	if ((pwm_period != -1) ||
	    (pwm_ontime != -1)) {
		if (pwm_ontime == -1) pwm_ontime = 0;
		if (pwm_period == -1) pwm_period = 0;
		if (maestro_compact_set_pwm(fd, (uint16_t)pwm_ontime, (uint16_t)pwm_period) < 0) {
			fprintf(stderr, "Failed to set PWM");
			return;
		}
	}
	
	/** Script cmds */
	if (stop) {
		if (maestro_compact_stop_script(fd) < 0){
			fprintf(stderr, "Failed to stop script");
			return;
		}
	}

	if (restart != -1) {
		if (parameter != -1) {
			if (maestro_compact_restart_script_par(fd, (uint8_t)restart, (uint16_t) parameter) < 0){
				fprintf(stderr, "Failed to restart script at subroutine %d with par %d\n", restart, parameter);
				return;
			}
		} else {			
			if (maestro_compact_restart_script(fd, (uint8_t)restart) < 0){
				fprintf(stderr, "Failed to restart script at subroutine %d\n", restart);
				return;
			}					
		}
	}

	
	
	/** Set targets */
	if (mult_num != -1) {
		if (mult_first != -1) {
			if (file) {
				uint16_t *targets_p = NULL;
				int32_t sz = get_targets_list(file, &targets_p);
				if (sz > 0) {
					if (sz < mult_num) {
						fprintf(stderr, "Number of targets in %s less then specified mult-num value %d\n", file, mult_num);
						free(targets_p);
						return;
					}
					
					if (maestro_compact_set_multiple_target(fd, 			                                        
					                                       (uint8_t) mult_num, 
					                                       (uint8_t) mult_first, 
					                                       targets_p) < 0) {
						fprintf(stderr, "Failed to set number of targets and first channel num\n");
						free(targets_p);
						return;
					}
					free(targets_p);
				}
			} else {
				fprintf(stderr, "Use --file for specifying file name with targets list\n");			
				return;				
			}
		} else {
			fprintf(stderr, "Use --mult-first for specifying first channel\n");			
			return;				
		}
	}
	
	if (target != -1) {

		if (ssc) {
			if (maestro_minissc_set_target(fd, (channel == -1) ? 0: (uint8_t) channel, (uint8_t) target) < 0) {
				fprintf(stderr, "Failed to set target\n");
				return;
			}
		} else {
			if (maestro_compact_set_target(fd, (channel == -1) ? 0: (uint8_t) channel, (uint16_t) target) < 0) {
				fprintf(stderr, "Failed to set target\n");
				return;
			}				
		}
	}

	
	/** Status cmds */
	
	if (is_stop) {
		int res = maestro_compact_is_stopped(fd, (timeout != -1) ? &tv : NULL);
		
		if (res == -1) {
			fprintf(stderr, "Failed to check script status\n");
			return;
		}

		if (res == 1) {
			fprintf(stdout, "Script is stopped\n");
		} else if (res == 0) {
			fprintf(stdout, "Script is running\n");
		}		
	}

	if (is_moving) {
		int res = maestro_compact_is_moving(fd, (timeout != -1) ? &tv : NULL);
		
		if (res == -1) {
			fprintf(stderr, "Failed to check moving status\n");
			return;
		}

		if (res == 1) {
			fprintf(stdout, "Some servo is moving\n");
		} else if (res == 0) {
			fprintf(stdout, "No one servo is moving\n");
		}		
	}

	if (get_position) {
		int res = maestro_compact_get_position(fd, (channel == -1) ? 0 : (uint8_t)channel, (timeout != -1) ? &tv : NULL);
		
		if (res == -1) {
			fprintf(stderr, "Failed to get position\n");
			return;
		}		
		fprintf(stdout, "POSITION: %u\n", (uint16_t)(res & 0xFFFF));				
	}

	if (get_errors) {
		int res = maestro_compact_get_errors(fd, (timeout != -1) ? &tv : NULL);
		
		if (res == -1) {
			fprintf(stderr, "Failed to check moving status\n");
			return;
		}		
		fprintf(stdout, "ERRORS: 0x%X\n", (uint16_t)(res & 0xFFFF));				
		pr_errors((uint16_t)(res & 0xFFFF));
	}	
	
	if (go_home) {
		if (maestro_compact_go_home(fd) < 0){
			fprintf(stderr, "Failed to set default home position");
			return;
		}
	}

}

static void exec_cmds_pololu (int32_t fd)
{
	/** Options */
	if (speed != -1) {
		if (maestro_pololu_set_speed(fd, (uint8_t)device, (channel == -1) ? 0 : (uint8_t)channel, (uint16_t)speed) < 0) {
			fprintf(stderr, "Failed to set speed");
			return;
		}
	}
	
	if (acceleration != -1) {
		if (maestro_pololu_set_acceleration(fd, (uint8_t)device, (channel == -1) ? 0 : (uint8_t)channel, (uint16_t)acceleration) < 0) {
			fprintf(stderr, "Failed to set acceleration");
			return;			
		}
	}

	if ((pwm_period != -1) ||
	    (pwm_ontime != -1)) {
		if (pwm_ontime == -1) pwm_ontime = 0;
		if (pwm_period == -1) pwm_period = 0;
		if (maestro_pololu_set_pwm(fd, (uint8_t)device, (uint16_t)pwm_ontime, (uint16_t)pwm_period) < 0) {
			fprintf(stderr, "Failed to set PWM");
			return;
		}
	}
	
	/** Script cmds */
	if (stop) {
		if (maestro_pololu_stop_script(fd, (uint8_t) device) < 0){
			fprintf(stderr, "Failed to stop script");
			return;
		}
	}

	if (restart != -1) {
		if (parameter != -1) {
			if (maestro_pololu_restart_script_par(fd, (uint8_t)device, (uint8_t)restart, (uint16_t) parameter) < 0){
				fprintf(stderr, "Failed to restart script at subroutine %d with par %d\n", restart, parameter);
				return;
			}
		} else {			
			if (maestro_pololu_restart_script(fd, (uint8_t)device, (uint8_t)restart) < 0){
				fprintf(stderr, "Failed to restart script at subroutine %d\n", restart);
				return;
			}					
		}
	}
	
	/** Set targets */
	if (mult_num != -1) {
		if (mult_first != -1) {
			if (file) {
				uint16_t *targets_p = NULL;
				int32_t sz = get_targets_list(file, &targets_p);
				if (sz > 0) {
					if (sz < mult_num) {
						fprintf(stderr, "Number of targets in %s less then specified mult-num value %d\n", file, mult_num);
						free(targets_p);
						return;
					}
					
					if (maestro_pololu_set_multiple_target(fd, 
					                                       (uint8_t) device, 
					                                       (uint8_t) mult_num, 
					                                       (uint8_t) mult_first, 
					                                       targets_p) < 0) {
						fprintf(stderr, "Failed to set number of targets and first channel num\n");
						free(targets_p);
						return;
					}
					free(targets_p);
				}
			} else {
				fprintf(stderr, "Use --file for specifying file name with targets list\n");			
				return;				
			}
		} else {
			fprintf(stderr, "Use --mult-first for specifying first channel\n");			
			return;				
		}
	}
		

	if (target != -1) {
		if (ssc) {
			if (maestro_minissc_set_target(fd, (channel == -1) ? 0: (uint8_t) channel, (uint8_t) target) < 0) {
				fprintf(stderr, "Failed to set target\n");
				return;
			}
		} else {
			if (maestro_pololu_set_target(fd, (uint8_t) device, (channel == -1) ? 0: (uint8_t) channel, (uint16_t) target) < 0) {
				fprintf(stderr, "Failed to set target\n");
				return;
			}				
		}
	}
	
	/** Status cmds */
	if (is_stop) {
		int res = maestro_pololu_is_stopped(fd, (uint8_t) device, (timeout != -1) ? &tv : NULL);
		
		if (res == -1) {
			fprintf(stderr, "Failed to check script status\n");
			return;
		}

		if (res == 1) {
			fprintf(stdout, "Script is stopped\n");
		} else if (res == 0) {
			fprintf(stdout, "Script is running\n");
		}		
	}

	if (is_moving) {
		int res = maestro_pololu_is_moving(fd, (uint8_t) device, (timeout != -1) ? &tv : NULL);
		
		if (res == -1) {
			fprintf(stderr, "Failed to check moving status\n");
			return;
		}

		if (res == 1) {
			fprintf(stdout, "Some servo is moving\n");
		} else if (res == 0) {
			fprintf(stdout, "No one servo is moving\n");
		}		
	}

	if (get_position) {
		int res = maestro_pololu_get_position(fd, (uint8_t) device, (channel == -1) ? 0 : (uint8_t)channel, (timeout != -1) ? &tv : NULL);
		
		if (res == -1) {
			fprintf(stderr, "Failed to check moving status\n");
			return;
		}		
		fprintf(stdout, "POSITION: %u\n", (uint16_t)(res & 0xFFFF));				
	}
	
	if (get_errors) {
		int res = maestro_pololu_get_errors(fd, (uint8_t) device, (timeout != -1) ? &tv : NULL);
		
		if (res == -1) {
			fprintf(stderr, "Failed to check moving status\n");
			return;
		}		
		fprintf(stdout, "ERRORS: 0x%X\n", (uint16_t)(res & 0xFFFF));				
		pr_errors((uint16_t)(res & 0xFFFF));
	}	
	
	if (go_home) {
		if (maestro_pololu_go_home(fd, (uint8_t) device) < 0){
			fprintf(stderr, "Failed to set default home position");
			return;
		}
	}

}

static void exec_cmds (void)
{
	int32_t fd;
	
	fd = maestro_open(device_file);   
	
	if (fd == -1) {
		fprintf(stderr, "Failed to open %s", device_file);
		return;
	}
	
	if (device != -1) { /** Work at Pololu protocol*/		
		exec_cmds_pololu(fd);
	} else { /** Work at Compact protocol*/
		exec_cmds_compact(fd);
	}

	maestro_close(fd);
}

static void pr_help (char* prog_name)
{
	printf("usage: %s [OPTIONS] [DEVICE]\n", prog_name);
	printf("List of options: \n");
	printf("\t --device,d NUM\t\t\t set device num and use Pololu protocol, default 0 and use Compact protocol\n");
	printf("\t --channel,c NUM\t\t set channel, default 0\n");
	printf("\t --target,t VALUE\t\t set target (0 - 16383 us, in 0.25 us units), default 0\n");
	printf("\t --ssc \t\t\t\t\t use MiniSSC protocol for setting target. Target value must be 8-bit. Value is interpreting relative to range of servo and Maestro Pololu settings.\n\n");

	printf("\t --speed,s VALUE\t\t set speed limit (in 0.025 us/ms units), 0 -- unlimited speed, default 0\n");
	printf("\t --acceleration,a VALUE\t set acceleration limit (in 0.025/80 us/(ms * ms) units), 0 -- unlimited acceleration, default 0\n");
	printf("\t --pwm_ontime VALUE\t\t set PWM on time (in 1/48 us units), default 0\n");
	printf("\t --pwm_period VALUE\t\t set PWM period (in 1/48 us units), default 0\n\n");



	printf("\t Setting multiple targets command: \n");
	printf("\t --mult_num NUM\t\t\t set multiple targets num, default 0\n");
	printf("\t --mult_first NUM\t\t set first channel for multiple targets command, default 0\n");
	printf("\t ...and you must use file with targets list: \n");
	printf("\t --file FILE \t\t\t set file source for list of targets\n\n");

	printf("\t Status commands: \n");
	printf("\t --get-position \t\t print current postion of servo\n");
	printf("\t --is-moving \t\t\t check if servo moving\n");
	printf("\t --get-errors \t\t\t print errors\n\n");
	printf("\t --go-home \t\t\t go default position\n\n");
	printf("\t --timeout \t\t\t set timeout for status commands (in ms)\n\n");

	printf("\t --stop \t\t\t\t stop script\n");
	printf("\t --restart NUM\t\t\t restart script at NUM subroutine\n");
	printf("\t --parameter NUM\t\t set parameter for restarting script\n");
	printf("\t --is-stop \t\t\t\t check if script stopped\n\n");
	printf("\t --help,h \t\t\t\t print this help and exit\n");

}

int32_t main(int32_t argc, char *argv[])
{
	int32_t c;   
	
	while (1) {
		int32_t option_index = 0;
		static struct option long_options[] = {
			{"device",   required_argument, 0,  'd' },
			{"channel",  required_argument, 0,  'c' },
			{"target",   required_argument, 0,  't' },
			{"ssc",    no_argument, 0,  0 },

			{"speed",    required_argument, 0,  's' },
			{"acceleration",    required_argument, 0,  'a' },
			{"pwm-ontime",    required_argument, 0,  0 },
			{"pwm-period",    required_argument, 0,  0 },
			

			{"mult-num",    required_argument, 0,  0 },
			{"mult-first",    required_argument, 0,  0 },
			{"file",    required_argument, 0,  0 },

			{"get-position",    no_argument, 0,  0 },
			{"is-moving",    no_argument, 0,  0 },
			{"get-errors",    no_argument, 0,  0 },
			{"go-home",    no_argument, 0,  0 },

			{"stop",    no_argument, 0,  0 },
			{"restart",    required_argument, 0,  0 },
			{"parameter",    required_argument, 0,  0 },
			{"timeout",    required_argument, 0,  0 },
			{"is-stop",    no_argument, 0,  0 },
			
			{"dev",    required_argument, 0,  0 },

			{"help",    no_argument, 0,  'h' },
			{0,         0,                 0,  0 }
		};

		c = getopt_long(argc, argv, "d:c:t:s:a:h",
		                long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'd':
			printf("\tdevice number %s\n", optarg);
			device = atoi(optarg);
			break;
		case 'c':
			printf("\tchannel number %s\n", optarg);
			channel = atoi(optarg);
			break;			
		case 't':
			target = atoi(optarg);
			printf("\ttarget %u\n", target);
			break;
		case 's':
			speed = atoi(optarg);
			printf("\tspeed %u\n", speed);
			break;
		case 'a':
			acceleration = atoi(optarg);
			printf("\tacceleration %u\n", acceleration);
			break;
			
		case 0:
			if (!strcmp(long_options[option_index].name, "pwm-ontime")) {
				pwm_ontime = atoi(optarg);
				printf("\tpwm ontime %u\n", pwm_ontime);
			} else if (!strcmp(long_options[option_index].name, "pwm-period")) {
				pwm_period = atoi(optarg);
				printf("\tpwm period %u\n", pwm_period);
			} else if (!strcmp(long_options[option_index].name, "get-position")) {
				get_position = 1;
				printf("\tget position\n");
			} else if (!strcmp(long_options[option_index].name, "get-errors")) {
				get_errors = 1;
				printf("\tget errors\n");
			} else if (!strcmp(long_options[option_index].name, "stop")) {
				stop = 1;
				printf("\tStopping script\n");
			} else if (!strcmp(long_options[option_index].name, "restart")) {
				restart = atoi(optarg);
				printf("\tRestarting at subroutine %d\n", restart);			
			} else if (!strcmp(long_options[option_index].name, "parameter")) {
				parameter = atoi(optarg);
				printf("\t Parameter for restarting %d\n", parameter);			
			} else if (!strcmp(long_options[option_index].name, "is-stop")) {
				is_stop = 1;
				printf("\tChecking if script stopped...\n");
			} else if (!strcmp(long_options[option_index].name, "is-moving")) {
				is_moving = 1;
				printf("\tChecking if servo is moving...\n");
			} else if (!strcmp(long_options[option_index].name, "mult-first")) {
				mult_first = atoi(optarg);
				printf("\tSet multiple targets, first channel %d\n", mult_first);
			} else if (!strcmp(long_options[option_index].name, "timeout")) {
				timeout = atoi(optarg);
				printf("\tTimeout %d\n", timeout);
				tv.tv_sec = timeout / 1000;
				tv.tv_usec = (timeout % 1000) * 1000;
				
			} else if (!strcmp(long_options[option_index].name, "ssc")) {
				ssc = 1;
				printf("\tSetting targets in MiniSSC protocol\n");
			} else if (!strcmp(long_options[option_index].name, "go-home")) {
				go_home = 1;
				printf("\tGo home\n");
			} else if (!strcmp(long_options[option_index].name, "mult-num")) {
				mult_num = atoi(optarg);
				printf("\tSet multiple targets, targets num %d\n", mult_num);
			} else if (!strcmp(long_options[option_index].name, "file")) {
				file = optarg;
				printf("\tFile with targets %s\n", file);
			} else if (!strcmp(long_options[option_index].name, "dev")) {
				device_file = optarg;
				printf("\tDevice file %s\n", device_file);
			} 
			break;			
		case 'h':
			pr_help(argv[0]);
			exit(EXIT_SUCCESS);
			break;
		case '?':
			break;

		default:
			printf("?? getopt returned character code 0%o ??\n", c);
		}
	}
		
	if (argc > 1) {
		exec_cmds();
	} else pr_help(argv[0]);
	
	exit(EXIT_SUCCESS);
}

