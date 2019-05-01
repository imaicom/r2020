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
	short *button;			// button[nr_buttons]
	short *stick;			// stick[nr_sticks]
};

#define NumberOfButton 16

int btn[NumberOfButton] = {};
int b_btn[NumberOfButton] = {};
int fds;
int mode = 10;
int btn_select = 0;
int b_btn_select = 0;
int btn_tri = 0;
int b_btn_tri = 0;
int btn_cir = 0;
int b_btn_cir = 0;
int btn_squ = 0;
int b_btn_squ = 0;
int btn_up = 0;
int b_btn_up = 0;
int btn_down = 0;
int b_btn_down = 0;
int btn_upd = 0;
int b_btn_upd = 0;
int btn_downd = 0;
int b_btn_downd = 0;
int btn_left = 0;
int b_btn_left = 0;
int btn_right = 0;
int b_btn_right = 0;
int btn_cross = 0;
int b_btn_cross = 0;
int btn_square = 0;
int b_btn_square = 0;
int btn_circle = 0;
int b_btn_circle = 0;
int btn_ps = 0;
int b_btn_ps = 0;
int btn_ljoy = 0;
int b_btn_ljoy = 0;
int btn_rjoy = 0;
int b_btn_rjoy = 0;

int btn_r1 = 0;
int b_btn_r1 = 0;
int btn_r2 = 0;
int b_btn_r2 = 0;
int btn_l1 = 0;
int b_btn_l1 = 0;
int btn_l2 = 0;
int b_btn_l2 = 0;

int l_mode = 3;
int l_mode_a = 60;//55
int l_mode_b = -80;
int r_mode = 3;
int r_mode_a = 72 ;
int r_mode_b = +90;
int t_mode = 0;
int h_mode = 12;
int d_mode = 1;
int lock = 0;
int petBottleLock = 0;
int megaPhonePush = 0;
int clawOn = 0;
int clawArc = +27;
int UpDown = 0;


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

	int i;
	unsigned char nr_btn = ps3dat->nr_buttons;
	unsigned char nr_stk = ps3dat->nr_sticks;
	int xx,yy,x,y,z,v1,v2,ww,c1,c2,c3,c4;

    c2 = ps3dat->stick[PAD_LEFT_Y] ;
    c4 = ps3dat->stick[PAD_RIGHT_Y];
    
    if ((1 - ps3dat->button[PAD_KEY_L_JOYSTICK])&&(1 - ps3dat->button[PAD_KEY_R_JOYSTICK])) {	// ゆっくり動く
		c2 = c2 / 30;
		c4 = c4 / 30;
	} else {
		c2 = c2 / 5; //10
		c4 = c4 / 5; //10
	};


    printf(" 左=%4d ",c2);
    printf(" 右=%4d ",c4);
    printf(" clawOn=%4d ",clawOn);
    printf("\n");
    printf("\n");


	if(abs(c2) < 5) {   // 右
		softPwmWrite(29,0); softPwmWrite(28,0);
	} else if(c2 > 0) {
		softPwmWrite(29,0); softPwmWrite(28,abs(c2));
	} else {
		softPwmWrite(29,abs(c2)); softPwmWrite(28,0);
	};


	if(abs(c4) < 5) {   // 左
		softPwmWrite(25,0); softPwmWrite(24,0);
	} else if(c4 > 0) {
		softPwmWrite(25,0); softPwmWrite(24,abs(c4));
	} else {
		softPwmWrite(25,abs(c4)); softPwmWrite(24,0);
	};


	if(ps3dat->button[PAD_KEY_SELECT]) btn[PAD_KEY_SELECT]++;
	if(!ps3dat->button[PAD_KEY_SELECT]) btn[PAD_KEY_SELECT] = 0;
	if(b_btn[PAD_KEY_SELECT] > btn[PAD_KEY_SELECT]) {
		softPwmWrite(22,50);
	};
	b_btn[PAD_KEY_SELECT] = btn[PAD_KEY_SELECT];


	if(ps3dat->button[PAD_KEY_SQUARE]) btn[PAD_KEY_SQUARE]++;
	if(!ps3dat->button[PAD_KEY_SQUARE]) btn[PAD_KEY_SQUARE] = 0;
	if(b_btn[PAD_KEY_SQUARE] > btn[PAD_KEY_SQUARE]) {
		
		megaPhonePush = 1 - megaPhonePush;
		
		if(megaPhonePush) {
			setPCA9685Duty(fds , 11 , +90);
		} else {
			setPCA9685Duty(fds , 11 , -100);
		};

	};
	b_btn[PAD_KEY_SQUARE] = btn[PAD_KEY_SQUARE];


	if(ps3dat->button[PAD_KEY_TRIANGLE]) btn[PAD_KEY_TRIANGLE]++;
	if(!ps3dat->button[PAD_KEY_TRIANGLE]) btn[PAD_KEY_TRIANGLE] = 0;
	if(b_btn[PAD_KEY_TRIANGLE] > btn[PAD_KEY_TRIANGLE]) {
		
		petBottleLock++; if(petBottleLock>2) petBottleLock = 0;
		
		if(petBottleLock == 0) setPCA9685Duty(fds ,  8 , -10);
		if(petBottleLock == 1) setPCA9685Duty(fds ,  8 , -70);
		if(petBottleLock == 2) setPCA9685Duty(fds ,  8 , -90);
	
	};
	b_btn[PAD_KEY_TRIANGLE] = btn[PAD_KEY_TRIANGLE];

	
	if(ps3dat->button[PAD_KEY_CIRCLE]) btn[PAD_KEY_CIRCLE]++;
	if(!ps3dat->button[PAD_KEY_CIRCLE]) btn[PAD_KEY_CIRCLE] = 0;
	if(b_btn[PAD_KEY_CIRCLE] > btn[PAD_KEY_CIRCLE]) {
		
		clawOn++; if(clawOn > 2) clawOn = 0;
		
		if(UpDown == 0) {
			if(clawOn == 0) system("mpg123 /home/pi/Music/servo-0.mp3 &");
			if(clawOn == 1) system("mpg123 /home/pi/Music/servo-1.mp3 &");
			if(clawOn == 2) system("mpg123 /home/pi/Music/servo-2.mp3 &");
		};
	};
	b_btn[PAD_KEY_CIRCLE] = btn[PAD_KEY_CIRCLE];
	

	if(UpDown == 0) {
		if(clawOn == 0) { setPCA9685Duty(fds ,  6 , +20); };
		if(clawOn == 1) { setPCA9685Duty(fds ,  6 , +30); };
		if(clawOn == 2) { setPCA9685Duty(fds ,  6 , +75); };
	};
	

	if(ps3dat->button[PAD_KEY_L1]) {
		 setPCA9685Duty(fds , 10 ,  +60);
	} else if(ps3dat->button[PAD_KEY_L2]) {
		 setPCA9685Duty(fds , 10 ,  -60);
	} else {
		setPCA9685Duty(fds , 10 ,  0);
	};



	if(ps3dat->button[PAD_KEY_R1]) {
		 setPCA9685Duty(fds , 7 ,  +60);
	} else if(ps3dat->button[PAD_KEY_R2]) {
		 setPCA9685Duty(fds , 7 ,  -60);
	} else {
		setPCA9685Duty(fds , 7 ,  0);
	};


	if((clawOn==0)||(clawOn==1)) {
		if(ps3dat->button[PAD_KEY_UP]) {
			setPCA9685Duty(fds , 0 ,  -100);UpDown = 0;
		} else if(ps3dat->button[PAD_KEY_DOWN]) {
			setPCA9685Duty(fds , 0 ,  +70); UpDown = 1;
		};
	};


	if(ps3dat->button[PAD_KEY_START]) {
		system("mpg123 /home/pi/Music/Safe_mode.mp3 &");
		softPwmWrite(5,0); // motor-1 10ms
		softPwmWrite(6,0); // motor-1 10ms
		softPwmWrite(26,0); // motor-2 10ms
		softPwmWrite(27,0); // motor-2 10ms
		softPwmWrite(28,0); // motor-3 10ms
		softPwmWrite(29,0); // motor-3 10ms
		softPwmWrite(24,0); // motor-4 10ms
		softPwmWrite(25,0); // motor-4 10ms
		softPwmWrite(14,0); // motor-5 10ms // NC
		softPwmWrite(23,0); // motor-5 10ms // NC
		softPwmWrite(3,0); // beep
		
		softPwmWrite(15,0); // motor-6 10ms
		softPwmWrite(16,0); // motor-6 10ms
		softPwmWrite(10,0); // motor-7 10ms
		softPwmWrite(11,0); // motor-7 10ms
		softPwmWrite(30,0); // motor-8 10ms
		softPwmWrite(31,0); // motor-8 10ms
		softPwmWrite(21,0); // motor-9 10ms
		softPwmWrite(22,0); // motor-9 10ms

		delay(1000);
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
	softPwmCreate(5,0,20); // motor-1 10ms
	softPwmCreate(6,0,20); // motor-1 10ms
	softPwmCreate(26,0,20); // motor-2 10ms
	softPwmCreate(27,0,20); // motor-2 10ms
	softPwmCreate(28,0,20); // motor-3 10ms
	softPwmCreate(29,0,20); // motor-3 10ms
	softPwmCreate(24,0,20); // motor-4 10ms
	softPwmCreate(25,0,20); // motor-4 10ms
	softPwmCreate(14,0,20); // motor-5 10ms // NC
	softPwmCreate(23,0,20); // motor-5 10ms // NC
	softPwmCreate(3,0,20); // beep
	
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
		
		setPCA9685Duty(fds , 0 ,  -100);
		UpDown = 0;
		setPCA9685Duty(fds , 1 ,  0);
		setPCA9685Duty(fds , 2 ,  0);
		setPCA9685Duty(fds , 3 ,  0);
		setPCA9685Duty(fds , 4 ,  0);
		setPCA9685Duty(fds , 5 ,  0);
		setPCA9685Duty(fds , 6 ,  +20);
		setPCA9685Duty(fds , 7 ,  0);
		setPCA9685Duty(fds , 9 ,  0);
		setPCA9685Duty(fds , 10 ,  0);
		setPCA9685Duty(fds , 11 ,  +90);
		setPCA9685Duty(fds , 12 ,  0);

		setPCA9685Duty(fds ,  8 , -10);

		do {
			if (ps3c_test(&ps3dat) < 0) break;
		} while (!(ps3c_input(&ps3dat)));

		ps3c_exit(&ps3dat);
	}
}

