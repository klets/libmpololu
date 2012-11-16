#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>
#include "mpololu.h" /* Maestro Pololu Lib */


int32_t device = -1;
int32_t channel = 0;

uint16_t target = 0;

uint16_t speed = 0;
uint16_t acceleration = 0;

uint16_t pwm_ontime = 0;
uint16_t pwm_period = 0;

int get_position = 0;
int get_errors = 0;
int ssc = 0;

int stop = 0;

int restart = -1;
int32_t parameter = -1;

int is_stop = 0;
int is_moving = 0;

int mult_num = -1;
int mult_first = -1;

char *file;

char *device_file = "/dev/ttyACM0";

static void pr_help (char* prog_name)
{
	printf("usage: %s [OPTIONS] [DEVICE]\n", prog_name);
	printf("List of options: \n");
	printf("\t --device,d NUM\t\t\t set device num and use Pololu protocol, default 0 and use Compact protocol\n");
	printf("\t --channel,c NUM\t\t set channel, default 0\n");
	printf("\t --target,t VALUE\t\t set target (0 - 16383 us), default 0\n");
	printf("\t --ssc \t\t\t\t\t use MiniSSC protocol for setting target\n\n");

	printf("\t --speed,s VALUE\t\t set speed limit (in 0.025 us/ms units), 0 -- unlimited speed, default 0\n");
	printf("\t --acceleration,a VALUE\t set acceleration limit (in 0.025/80 us/(ms * ms) units), 0 -- unlimited acceleration, default 0\n");
	printf("\t --pwm_ontime VALUE\t\t set PWM on time (in 1/48 us units), default 0\n");
	printf("\t --pwm_period VALUE\t\t set PWM period (in 1/48 us units), default 0\n\n");



	printf("\t Setting multiple targets command: \n");
	printf("\t --mult_num NUM\t\t\t set multiple targets num, default 0\n");
	printf("\t --mult_first NUM\t\t set first channel for multiple targets command, default 0\n");
	printf("\t ...then you can use file with targets list or this program with only \"--target,t\" option: \n");
	printf("\t --file FILE \t\t\t set file source for list of targets\n\n");

	printf("\t --get-position \t\t print current postion of servo\n");
	printf("\t --is-moving \t\t\t check if servo moving\n");
	printf("\t --get-errors \t\t\t print errors\n\n");

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

			{"stop",    no_argument, 0,  0 },
			{"restart",    required_argument, 0,  0 },
			{"parameter",    required_argument, 0,  0 },
			{"is-stop",    no_argument, 0,  0 },
			
			{"help",    no_argument, 0,  'h' },
			{0,         0,                 0,  0 }
		};

		c = getopt_long(argc, argv, "",
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
				printf("\tFirstly get position\n");
			} else if (!strcmp(long_options[option_index].name, "get-errors")) {
				get_errors = 1;
				printf("\tFirstly get errors\n");
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
			} else if (!strcmp(long_options[option_index].name, "mult-num")) {
				mult_num = atoi(optarg);
				printf("\tSet multiple targets, targets num %d\n", mult_num);
			} else if (!strcmp(long_options[option_index].name, "file")) {
				file = optarg;
				printf("\tFile with targets %s\n", file);
			} 
			
	
			if (optarg)
				printf(" with arg %s", optarg);
			printf("\n");
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

	if (optind < argc) {
		printf("Device file: ");
		device_file = argv[optind];
		printf("%s ", device_file);
		printf("\n");
	}

	exit(EXIT_SUCCESS);
}

