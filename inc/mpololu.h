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
	/*                            FUNCTION DECLARATIONS                       */
	/**************************************************************************/



	/** Common API */

	/**
	 * @brief Open serial interface
	 *  
	 * @details works +
	 *
	 * @param device -- name of COM-port device file
	 *
	 * @retval File desriptor of opened interface or -1 if error occured
	 */
	int32_t maestro_open(const char* device);

	/**
	 * @brief Close serial interface
	 *	
	 * @details works +
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
	 * @details works +
	 *
	 * @param fd -- file descriptor of opened COM-port
	 * @param device -- device number
	 * @param channel -- device channel number
	 * @param target -- absolute angle of rotation in 0.25 us
	 *
	 * @retval 0 -- success, -1 -- failed
	 */
	int32_t maestro_pololu_set_target(int32_t fd, uint8_t device, uint8_t channel, uint16_t target);

	/**
	 * @brief Maestro set target (Compact protocol)
	 *
	 * @details works +
	 *
	 * @param fd -- file descriptor of opened COM-port
	 * @param channel -- device channel number
	 * @param target -- absolute angle of rotation in 0.25 us
	 *
	 * @retval 0 -- success, -1 -- failed
	 */
	int32_t maestro_compact_set_target(int32_t fd, uint8_t channel, uint16_t target);

	/**
	 * @brief Maestro set target (MiniSSC protocol)
	 * 
	 * @details works +.  The 8-bit target value is converted to a full-resolution target value according to 
	 * the range and neutral settings stored on the Maestro for that channel. Specifically, an 8-bit target 
	 * of 127 corresponds to the neutral setting for that channel, while 0 or 254 correspond to the neutral
	 * setting minus or plus the range setting. These settings can be useful for calibrating motion without
	 * changing the program sending serial commands.
	 *
	 * @param fd -- file descriptor of opened COM-port
	 * @param channel -- device channel number
	 * @param target -- 8-bit target value. Value is interprets relative to range of servo
	 * and Maestro Pololu min/max settings.
	 *
	 * @retval 0 -- success, -1 -- failed	 
	 */
	int32_t maestro_minissc_set_target(int32_t fd, uint8_t channel, uint8_t target);



	/** Set multiple target */
	/**
	 * @brief Maestro set multiple target (Pololu protocol)
	 * works+
	 */
	int32_t maestro_pololu_set_multiple_target(int32_t fd, uint8_t device, uint8_t targets_num, uint8_t first_channel, uint16_t* targets_p);

	/**
	 * @brief Maestro set multiple target (Compact protocol)
	 * works+
	 */
	int32_t maestro_compact_set_multiple_target(int32_t fd, uint8_t targets_num, uint8_t first_channel, uint16_t* targets_p);


	/** Other commands */

	/**
	 *  @brief Set speed
	 * works+
	 */
	int32_t maestro_pololu_set_speed(int32_t fd, uint8_t device, uint8_t channel, uint16_t speed);
	int32_t maestro_compact_set_speed(int32_t fd, uint8_t channel, uint16_t speed);

	/**
	 *  @brief Set acceleration
	 * works+
	 */
	int32_t maestro_pololu_set_acceleration(int32_t fd, uint8_t device, uint8_t channel, uint16_t acceleration);
	int32_t maestro_compact_set_acceleration(int32_t fd, uint8_t channel, uint16_t acceleration);

	/**
	 *  @brief Set PWM
	 * not tested
	 */
	int32_t maestro_pololu_set_pwm(int32_t fd, uint8_t device, uint16_t on_time, uint16_t period);
	int32_t maestro_compact_set_pwm(int32_t fd, uint16_t on_time, uint16_t period);

	/**
	 *  @brief Get position
	 *  
	 *  @retval 16 bit position value
	 */
	int32_t maestro_pololu_get_position(int32_t fd, uint8_t device, uint8_t channel, struct timeval* timeout);
	int32_t maestro_compact_get_position(int32_t fd, uint8_t channel, struct timeval* timeout);

	/**
	 *  @brief Get moving state
	 *
	 *  @retval 0 - if no servos are moving, 1 - otherwise.
	 */
	int32_t maestro_pololu_is_moving(int32_t fd, uint8_t device, struct timeval* timeout);
	int32_t maestro_compact_is_moving(int32_t fd, struct timeval* timeout);

	/**
	 *  @brief Get errors
	 * 
	 *  @retval 16 bit error value
	 */
	int32_t maestro_pololu_get_errors(int32_t fd, uint8_t device, struct timeval* timeout);
	int32_t maestro_compact_get_errors(int32_t fd, struct timeval* timeout);

	/**
	 *  @brief Go home
	 */
	int32_t maestro_pololu_go_home(int32_t fd, uint8_t device);
	int32_t maestro_compact_go_home(int32_t fd);



	/** Scripts API */
	/**
	 *  @brief Stop script
	 */
	int32_t maestro_pololu_stop_script(int32_t fd, uint8_t device);
	int32_t maestro_compact_stop_script(int32_t fd);

	/**
	 *  @brief Restart script at subroutine
	 */
	int32_t maestro_pololu_restart_script(int32_t fd, uint8_t device, uint8_t subroutine_number);
	int32_t maestro_compact_restart_script(int32_t fd, uint8_t subroutine_number);

	/**
	 *  @brief Restart script at subroutine with parameter
	 */
	int32_t maestro_pololu_restart_script_par(int32_t fd, uint8_t device, uint8_t subroutine_number, uint16_t parameter);
	int32_t maestro_compact_restart_script_par(int32_t fd, uint8_t subroutine_number, uint16_t parameter);

	/**
	 *  @brief Get script status
	 *  @retval 0 - script is running, 1 - script is stopped
	 */
	int32_t maestro_pololu_is_stopped(int32_t fd, uint8_t device, struct timeval* timeout);
	int32_t maestro_compact_is_stopped(int32_t fd, struct timeval* timeout);

#ifdef __cplusplus
}
#endif

#endif /* MPOLOLU_H */
