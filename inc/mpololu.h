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
	 *  @retval File desriptor of opened interface
	 */
	int32_t maestro_open(const char* device);

	/**
	 * @brief Close serial interface
	 *  
	 */
	int32_t maestro_close(int32_t fd);



	/** Serial commands API */

	/** Set target */

	/**
	 * @brief Maestro set target (Pololu protocol)
	 */
	int32_t maestro_pololu_set_target(int32_t fd, uint8_t device, uint8_t channel, uint16_t target);

	/**
	 * @brief Maestro set target (Compact protocol)
	 */
	int32_t maestro_compact_set_target(int32_t fd, uint8_t channel, uint16_t target);

	/**
	 * @brief Maestro set target (MiniSSC protocol)
	 */
	int32_t maestro_minissc_set_target(int32_t fd, uint8_t channel, uint16_t target);



	/** Set multiple target */
	/**
	 * @brief Maestro set multiple target (Pololu protocol)
	 * 
	 * For sending target bytes use maestro_send_target() function repeatedly.
	 */
	int32_t maestro_pololu_set_multiple_target(int32_t fd, uint8_t device, uint8_t targets_num, uint8_t first_channel);

	/**
	 * @brief Maestro set multiple target (Compact protocol)
	 * 
	 * For sending target bytes use maestro_send_target() function repeatedly.
	 */
	int32_t maestro_compact_set_multiple_target(int32_t fd, uint8_t targets_num, uint8_t first_channel);

	/**
	 *  @brief Just sending target bytes
	 */
	int32_t maestro_send_target(int32_t fd, uint16_t target);



	/** Other commands */

	/**
	 *  @brief Set speed
	 */
	int32_t maestro_pololu_set_speed(int32_t fd, uint8_t device, uint8_t channel, uint16_t speed);
	int32_t maestro_compact_set_speed(int32_t fd, uint8_t channel, uint16_t speed);

	/**
	 *  @brief Set acceleration
	 */
	int32_t maestro_pololu_set_acceleration(int32_t fd, uint8_t device, uint8_t channel, uint16_t acceleration);
	int32_t maestro_compact_set_acceleration(int32_t fd, uint8_t channel, uint16_t acceleration);

	/**
	 *  @brief Set PWM
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
