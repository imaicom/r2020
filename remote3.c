// cc -o remote remote.c -lwiringPi -lm

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

#include <wiringPi.h>
#include <softPwm.h>

#include "controller.h"
#include <math.h>


struct ps3ctls {

	int fd;
	unsigned char nr_buttons;	// Max number of Buttons
	unsigned char nr_sticks;	// Max number of Sticks
	short *button;				// button[nr_buttons]
	short *stick;					// stick[nr_sticks]
};


#define NumberOfButton 17


int btn[NumberOfButton] = {};
int b_btn[NumberOfButton] = {};
int fds;

int ready_Go = 0;
int tennisBallCatch = 0;
int tennisBallCatch2 = 0;
int dropGate = 0;

int mode = 0;


int resetPCA9685(int fd) {
	wiringPiI2CWriteReg8(fd,0,0);
}


int setPCA9685Freq(int fd , float freq) {
	float prescaleval;
	int prescale , oldmode , newmode;
	freq = 0.9 * freq;
	prescaleval = 25000000.0;
	prescaleval /= 4096.0;
	prescaleval /= freq;
	prescaleval -= 1.0;
	prescale = prescaleval + 0.5;
	oldmode = wiringPiI2CReadReg8(fd,0x00);
	newmode = (oldmode & 0x7F)|0x10;
	wiringPiI2CWriteReg8(fd , 0x00 , newmode);
	wiringPiI2CWriteReg8(fd , 0xFE , prescale);
	wiringPiI2CWriteReg8(fd , 0x00 , oldmode);
	sleep(0.005);
	wiringPiI2CWriteReg8(fd , 0x00 , oldmode | 0xA1);
}


int setPCA9685Duty(int fd , int channel , int off) {
	int channelpos;
	int on;

	on   = 0;
	off += 276;
	channelpos = 0x6 + 4 * channel;
	wiringPiI2CWriteReg16(fd , channelpos   , on  & 0x0FFF);
	wiringPiI2CWriteReg16(fd , channelpos+2 , off & 0x0FFF);
}


int ps3c_test(struct ps3ctls *ps3dat) {

	unsigned char nr_btn = ps3dat->nr_buttons;
	unsigned char nr_stk = ps3dat->nr_sticks;

	int xx,yy,x,y,z,v1,v2,ww,c1,c2,c3,c4;



// Drive train


    v1 = ps3dat->stick [PAD_LEFT_Y];
    v2 = ps3dat->stick [PAD_RIGHT_Y];
    
    c1= -v1 / 2.0;
    c2= -v2 / 2.0;
    
    printf(" 0=%4d ",c1);
    printf(" 1=%4d ",c2);
    printf("\n");
    printf(" 2=%4d ",c3);
    printf(" 3=%4d ",c4);
    printf("\n");
    printf("\n");
    
    
    if(c1 > +5) {
		softPwmWrite( 5, abs(c1));	softPwmWrite( 6, 0); // left Front
		softPwmWrite(28, abs(c1));	softPwmWrite(29, 0); // left Rear
	} else if(c1 < -5) 	{
		softPwmWrite( 5, 0); 		softPwmWrite( 6, abs(c1));
		softPwmWrite(28, 0); 		softPwmWrite(29, abs(c1));
	} else {
		softPwmWrite( 5, 0);		softPwmWrite( 6, 0);
		softPwmWrite(28, 0);		softPwmWrite(29, 0);
	};
    
     if(c2 > +5) {
		softPwmWrite(26, 0);		softPwmWrite(27, abs(c2)); // right Front
		softPwmWrite( 1, 0); 		softPwmWrite( 4, abs(c2)); // right Rear
	} else if(c2 < -5) 	{
		softPwmWrite(26, abs(c2)); 	softPwmWrite(27, 0);
		softPwmWrite( 1, abs(c2)); 	softPwmWrite( 4, 0);
	} else {
		softPwmWrite(26, 0);		softPwmWrite(27, 0);
		softPwmWrite( 1, 0);		softPwmWrite( 4, 0);
	};
    
      
// end of Drive train
		
		
	if((ps3dat->button[PAD_KEY_PS])&&(!ready_Go)) {
		ready_Go = 1;
		system("mpg123 /home/pi/Music/ready_Go.mp3");
	};
	

	if(ps3dat->button[PAD_KEY_START]) {
		system("mpg123 /home/pi/Music/turnOff.mp3");
		softPwmWrite( 5,0); // motor-1 10ms
		softPwmWrite( 6,0); // motor-1 10ms
		softPwmWrite(26,0); // motor-2 10ms
		softPwmWrite(27,0); // motor-2 10ms
		softPwmWrite( 1,0); // motor-3 10ms
		softPwmWrite( 4,0); // motor-3 10ms
		softPwmWrite(28,0); // motor-4 10ms
		softPwmWrite(29,0); // motor-4 10ms
		softPwmWrite(25,0); // motor-5 10ms // NC
		softPwmWrite(24,0); // motor-5 10ms // NC
		softPwmWrite(3,0); // beep

		softPwmWrite(15,0); // motor-6 10ms
		softPwmWrite(16,0); // motor-6 10ms
		softPwmWrite(10,0); // motor-7 10ms
		softPwmWrite(11,0); // motor-7 10ms
		softPwmWrite(30,0); // motor-8 10ms
		softPwmWrite(31,0); // motor-8 10ms
		softPwmWrite(21,0); // motor-9 10ms
		softPwmWrite(22,0); // motor-9 10ms
		system("mpg123 /home/pi/Music/byeBye.mp3");

		return -1; // end of program
	};

	return 0;

}	//	int ps3c_test(struct ps3ctls *ps3dat)


int ps3c_input(struct ps3ctls *ps3dat) {

	int rp;
	struct js_event ev;

	do {
		rp = read(ps3dat->fd, &ev, sizeof(struct js_event));
		if (rp != sizeof(struct js_event)) {
			return -1;
		}
	} while (ev.type & JS_EVENT_INIT);

	switch (ev.type) {
		case JS_EVENT_BUTTON:
			if (ev.number < ps3dat->nr_buttons) {
				ps3dat->button[ev.number] = ev.value;
			}
			break;
		case JS_EVENT_AXIS:
			if (ev.number < ps3dat->nr_sticks) {
				ps3dat->stick[ev.number] = ev.value / 200; // 327 range -32767 ~ +32768 -> -100 ~ +100
			}
			break;
		default:
			break;
	}

	return 0;
}


int ps3c_getinfo(struct ps3ctls *ps3dat) {

	if(ioctl(ps3dat->fd , JSIOCGBUTTONS , &ps3dat->nr_buttons) < 0) return -1;
	if(ioctl(ps3dat->fd , JSIOCGAXES    , &ps3dat->nr_sticks ) < 0) return -2;

	return 0;
}


int ps3c_init(struct ps3ctls *ps3dat, const char *df) {

	unsigned char nr_btn;
	unsigned char nr_stk;
	unsigned char *p;
	int i;

	ps3dat->fd = open(df, O_RDONLY);
	if (ps3dat->fd < 0) return -1;

	if (ps3c_getinfo(ps3dat) < 0) {
		close(ps3dat->fd);
		return -2;
	}

	nr_btn = ps3dat->nr_buttons;
	nr_stk = ps3dat->nr_sticks;

	p = calloc(nr_btn + nr_stk , sizeof(short));
	if (p == NULL) {
		close(ps3dat->fd);
		return -3;
	}
	ps3dat->button = (short *)p;
	ps3dat->stick  = (short *)&p[nr_btn * sizeof(short)];

	return 0;
}

void ps3c_exit   (struct ps3ctls *ps3dat) {

	free (ps3dat->button);
	close(ps3dat->fd);
}


void main() {

	char *df = "/dev/input/js0";
	struct ps3ctls ps3dat;

	wiringPiSetup();
	softPwmCreate( 5,0,20); // motor-1 10ms
	softPwmCreate( 6,0,20); // motor-1 10ms
	softPwmCreate(26,0,20); // motor-2 10ms
	softPwmCreate(27,0,20); // motor-2 10ms
	softPwmCreate( 1,0,20); // motor-3 10ms
	softPwmCreate( 4,0,20); // motor-3 10ms
	softPwmCreate(28,0,20); // motor-4 10ms
	softPwmCreate(29,0,20); // motor-4 10ms
	softPwmCreate(25,0,20); // motor-5 10ms
	softPwmCreate(24,0,20); // motor-5 10ms
	softPwmCreate( 3,0,20); // beep

	softPwmCreate(15,0,20); // motor-6 10ms
	softPwmCreate(16,0,20); // motor-6 10ms
	softPwmCreate(10,0,20); // motor-7 10ms
	softPwmCreate(11,0,20); // motor-7 10ms
	softPwmCreate(30,0,20); // motor-8 10ms
	softPwmCreate(31,0,20); // motor-8 10ms
	softPwmCreate(21,0,20); // motor-9 10ms
	softPwmCreate(22,0,20); // motor-9 10ms

	fds = wiringPiI2CSetup(0x40);	// PCA9685
	resetPCA9685(fds);
	setPCA9685Freq(fds,50);
	system("mpg123 /home/pi/Music/Main_system_startup.mp3");
	delay(200);
	system("mpg123 /home/pi/Music/Press_the_PS_button.mp3");


	if(!(ps3c_init(&ps3dat, df))) {

		setPCA9685Duty(fds , 0 ,   0);
		setPCA9685Duty(fds , 1 ,   0);
		setPCA9685Duty(fds , 2 ,   0);
		setPCA9685Duty(fds , 3 ,   0);
		setPCA9685Duty(fds , 4 ,   0);
		setPCA9685Duty(fds , 5 ,   0);
		setPCA9685Duty(fds , 6 ,   0);
		setPCA9685Duty(fds , 7 ,   0);
		setPCA9685Duty(fds , 8 ,   0);
		setPCA9685Duty(fds , 9 ,   0);
		setPCA9685Duty(fds , 10,   0);
		setPCA9685Duty(fds , 11,   0);
		setPCA9685Duty(fds , 12,   0);
		setPCA9685Duty(fds , 13,   0);


		do {
			if (ps3c_test(&ps3dat) < 0) break;
		} while (!(ps3c_input(&ps3dat)));

		ps3c_exit(&ps3dat);
	}
}

