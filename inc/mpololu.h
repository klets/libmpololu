/**
 * @file   mpololu.h
 * @Author kls (gbkletsko@gmail.com)
 * @date   November, 2012
 * @brief  API for communicating with Maestro Pololu.
 *
 */
#ifndef MPOLOLU_H
#define MPOLOLU_H

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif
	/**************************************************************************/
	/*                                 ERROR CODES                            */
	/**************************************************************************/


#define POLOLU_ERR_SIG (0x1) 	/** Serial Signal Error */
#define POLOLU_ERR_OVR (0x2)  /** Serial Overrun Error */
#define POLOLU_ERR_RX (0x4)  /** Serial RX buffer full */
#define POLOLU_ERR_CRC (0x8)  /** Serial CRC error */

#define POLOLU_ERR_PROTO (0x10)  /** Serial protocol error */
#define POLOLU_ERR_TIMEOUT (0x20)  /** Serial timeout error */
#define POLOLU_ERR_STACK (0x40)  /** Serial stack error */
#define POLOLU_ERR_CALLSTACK (0x80)  /** Serial call stack error */

#define POLOLU_ERR_COUNTER (0x100)  /** Serial program counter error */
	

	/**************************************************************************/
	/*                            FUNCTION DECLARATIONS                       */
	/**************************************************************************/

	

	/** Common API */

	/**
	 * @brief Open serial interface
	 *
	 * @param device -- name of COM-port device file
	 *
	 * @retval File desriptor of opened interface or -1 if error occured
	 */
	int32_t maestro_open(const char* device);

	/**
	 * @brief Close serial interface
	 *
	 * @param fd -- file dsecriptor of opened COM-port
	 *
	 * @retval 0 -- success, -1 -- failed
	 */
	int32_t maestro_close(int32_t fd);



	/** Serial commands API */

	/** Set target */

	/**
	 * @brief Maestro set target (Pololu protocol)
	 *
	 * @param fd -- file descriptor of opened COM-port
	 * @param device -- device number
	 * @param channel -- device channel number
	 * @param target -- absolute angle of rotation in 0.25 us units
	 *
	 * @retval 0 -- success, -1 -- failed
	 */
	int32_t maestro_pololu_set_target(int32_t fd, uint8_t device, uint8_t channel, uint16_t target);

	/**
	 * @brief Maestro set target (Compact protocol)
	 *
	 * @details 
	 *
	 * @param fd -- file descriptor of opened COM-port
	 * @param channel -- device channel number
	 * @param target -- absolute angle of rotation in 0.25 us units
	 *
	 * @retval 0 -- success, -1 -- failed
	 */
	int32_t maestro_compact_set_target(int32_t fd, uint8_t channel, uint16_t target);

	/**
	 * @brief Maestro set target (MiniSSC protocol)
	 * 
	 * @details The 8-bit target value is converted to a full-resolution target value according to 
	 * the range and neutral settings stored on the Maestro for that channel. Specifically, an 8-bit target 
	 * of 127 corresponds to the neutral setting for that channel, while 0 or 254 correspond to the neutral
	 * setting minus or plus the range setting. These settings can be useful for calibrating motion without
	 * changing the program sending serial commands.
	 *
	 * @param fd -- file descriptor of opened COM-port
	 * @param channel -- device channel number plus device SSC offset
	 * @param target -- 8-bit target value. Value is interprets relative to range of servo
	 * and Maestro Pololu min/max settings.
	 *
	 * @retval 0 -- success, -1 -- failed	 
	 */
	int32_t maestro_minissc_set_target(int32_t fd, uint8_t channel, uint8_t target);



	/** Set multiple target */
	/**
	 * @brief Maestro set multiple target (Pololu protocol)
	 * 
	 * @details Simultaneously set multiple servo to wanted targets.
	 *
	 * @param fd -- file descriptor of opened COM-port
	 * @param device -- device number
	 * @param targets_num -- number of targets
	 * @param firs_channel -- number of first channel to set
	 * @param targets_p -- pointer to array of targets
	 *
	 * @retval 0 -- success, -1 -- failed
	 * 
	 */
	int32_t maestro_pololu_set_multiple_target(int32_t fd, uint8_t device, uint8_t targets_num, uint8_t first_channel, uint16_t* targets_p);

	/**
	 * @brief Maestro set multiple target (Compact protocol)
	 *
	 * @details Simultaneously set multiple servo to wanted targets.
	 *
	 * @param fd -- file descriptor of opened COM-port
	 * @param targets_num -- number of targets
	 * @param firs_channel -- number of first channel to set
	 * @param targets_p -- pointer to array of targets
	 *
	 * @retval 0 -- success, -1 -- failed
	 * 	 
	 */
	int32_t maestro_compact_set_multiple_target(int32_t fd, uint8_t targets_num, uint8_t first_channel, uint16_t* targets_p);


	/** Other commands */

	/**
	 * @brief Set speed
	 *
	 * @details Set speed limit (in 0.025 us/ms units), 0 -- unlimited speed, default 0
	 *
	 * @param fd -- file descriptor of opened COM-port
	 * @param device -- device number
	 * @param channel -- device channel number
	 * @param speed -- in 0.025 us/ms units), 0 -- unlimited speed
	 *
	 * @retval 0 -- success, -1 -- failed
	 *
	 */
	int32_t maestro_pololu_set_speed(int32_t fd, uint8_t device, uint8_t channel, uint16_t speed);
	int32_t maestro_compact_set_speed(int32_t fd, uint8_t channel, uint16_t speed);

	/**
	 * @brief Set acceleration
	 *
	 * @details Set acceleration limit (in 0.025/80 us/(ms * ms) units), 0 -- unlimited acceleration
	 *
	 * @param fd -- file descriptor of opened COM-port
	 * @param device -- device number
	 * @param channel -- device channel number
	 * @param acceleration -- acceleration limit in 0.025/80 us/(ms * ms) units, 0 -- unlimited acceleration
	 *
	 * @retval 0 -- success, -1 -- failed
	 *
	 */
	int32_t maestro_pololu_set_acceleration(int32_t fd, uint8_t device, uint8_t channel, uint16_t acceleration);
	int32_t maestro_compact_set_acceleration(int32_t fd, uint8_t channel, uint16_t acceleration);

	/**
	 * @brief Set PWM
	 * 
	 * @details Not tested.
	 *
	 * @param fd -- file descriptor of opened COM-port
	 * @param device -- device number
	 * @param on_time -- on time part of period (in 1/48 us units), default 0
	 * @param period -- value of period (in 1/48 us units), default 0
	 *
	 * @retval 0 -- success, -1 -- failed
	 *
	 */
	int32_t maestro_pololu_set_pwm(int32_t fd, uint8_t device, uint16_t on_time, uint16_t period);
	int32_t maestro_compact_set_pwm(int32_t fd, uint16_t on_time, uint16_t period);

	/**
	 * @brief Get position
	 *
	 * @details Implemented through select()
	 *
	 * @param fd -- file descriptor of opened COM-port
	 * @param device -- device number
	 * @param channel -- device channel number
	 * @param timeout -- pointer to timeout value, if NULL -- infinite timeout
	 *  
	 * @retval 16 bit position value in 0.25 us units, -1 -- if error occured
	 *
	 */
	int32_t maestro_pololu_get_position(int32_t fd, uint8_t device, uint8_t channel, struct timeval* timeout);
	int32_t maestro_compact_get_position(int32_t fd, uint8_t channel, struct timeval* timeout);

	/**
	 * @brief Get moving state
	 *
	 * @details Implemented through select()
	 *
	 * @param fd -- file descriptor of opened COM-port
	 * @param device -- device number   
	 * @param timeout -- pointer to timeout value, if NULL -- infinite timeout
	 *
	 * @retval 0 - if no servos are moving, 1 - otherwise, -1 -- if error
	 */
	int32_t maestro_pololu_is_moving(int32_t fd, uint8_t device, struct timeval* timeout);
	int32_t maestro_compact_is_moving(int32_t fd, struct timeval* timeout);

	/**
	 * @brief Get errors
	 * 
	 * @details Implemented through select()
	 *
	 * @param fd -- file descriptor of opened COM-port
	 * @param device -- device number   
	 * @param timeout -- pointer to timeout value, if NULL -- infinite timeout
	 *
	 * @retval 16 bit error value, -1 -- if failed, see POLOLU_ERR_* macro for error explanation
	 *
	 */
	int32_t maestro_pololu_get_errors(int32_t fd, uint8_t device, struct timeval* timeout);
	int32_t maestro_compact_get_errors(int32_t fd, struct timeval* timeout);

	/**
	 * @brief Go home
	 *
	 * @param fd -- file descriptor of opened COM-port
	 * @param device -- device number   
	 *
	 * @retval 0 -- success, -1 -- if failed
	 */
	int32_t maestro_pololu_go_home(int32_t fd, uint8_t device);
	int32_t maestro_compact_go_home(int32_t fd);



	/** Scripts API */
	/**
	 * @brief Stop script
	 *
	 * @param fd -- file descriptor of opened COM-port
	 * @param device -- device number   
	 *
	 * @retval 0 -- success, -1 -- if failed
	 *
	 */
	int32_t maestro_pololu_stop_script(int32_t fd, uint8_t device);
	int32_t maestro_compact_stop_script(int32_t fd);

	/**
	 * @brief Restart script at subroutine
	 *
	 * @param fd -- file descriptor of opened COM-port
	 * @param device -- device number   
	 * @param subroutine_number -- number of subroutine where script must be restarted
	 *
	 * @retval 0 -- success, -1 -- if failed
	 *
	 */
	int32_t maestro_pololu_restart_script(int32_t fd, uint8_t device, uint8_t subroutine_number);
	int32_t maestro_compact_restart_script(int32_t fd, uint8_t subroutine_number);

	/**
	 * @brief Restart script at subroutine with parameter
	 * 
	 * @param fd -- file descriptor of opened COM-port
	 * @param device -- device number   
	 * @param subroutine_number -- number of subroutine where script must be restarted
	 * @param parameter -- parameter to subroutine, the parameter must be between 0 and 16383
	 *
	 * @retval 0 -- success, -1 -- if failed
	 *
	 */
	int32_t maestro_pololu_restart_script_par(int32_t fd, uint8_t device, uint8_t subroutine_number, uint16_t parameter);
	int32_t maestro_compact_restart_script_par(int32_t fd, uint8_t subroutine_number, uint16_t parameter);

	/**
	 * @brief Get script status
	 *
	 * @param fd -- file descriptor of opened COM-port
	 * @param device -- device number   
	 * @param timeout -- pointer to timeout value, if NULL -- infinite timeout
	 *
	 * @retval 0 - script is running, 1 - script is stopped
	 */
	int32_t maestro_pololu_is_stopped(int32_t fd, uint8_t device, struct timeval* timeout);
	int32_t maestro_compact_is_stopped(int32_t fd, struct timeval* timeout);

#ifdef __cplusplus
}
#endif

#endif /* MPOLOLU_H */
