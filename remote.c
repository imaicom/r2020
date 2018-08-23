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
int megaPhoneLock = 0;


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

    c2 = (ps3dat->stick[PAD_LEFT_Y] )*(1-ps3dat->button[PAD_KEY_R2]);
    c4 = (ps3dat->stick[PAD_RIGHT_Y])*(1-ps3dat->button[PAD_KEY_R2]);
    
    if (1 - ps3dat->button[PAD_KEY_L2]) {	// ゆっくり動く
		c2 = c2 / 15;
		c4 = c4 / 15;
	};


    printf(" 左=%4d ",c2);
    printf(" 右=%4d ",c4);
    printf(" d_mode=%4d ",d_mode);
    printf(" L_JOYSTICK=%4d ",ps3dat->button[PAD_KEY_L_JOYSTICK]);
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


	if(ps3dat->button[PAD_KEY_SELECT]) btn_select++;
	if(!ps3dat->button[PAD_KEY_SELECT]) btn_select = 0;
	if(b_btn_select > btn_select) {
	
	};
	b_btn_select = btn_select;


	if(ps3dat->button[PAD_KEY_L1]) btn_l1++;
	if(!ps3dat->button[PAD_KEY_L1]) btn_l1 = 0;
	if(b_btn_l1 > btn_l1) {

	};
	b_btn_l1 = btn_l1;


	if(ps3dat->button[PAD_KEY_R1]) btn_r1++;
	if(!ps3dat->button[PAD_KEY_R1]) btn_r1 = 0;
	if(b_btn_r1 > btn_r1) {


	};
	b_btn_r1 = btn_r1;


	if(ps3dat->button[PAD_KEY_SQUARE]) btn_square++;
	if(!ps3dat->button[PAD_KEY_SQUARE]) btn_square = 0;
	if(b_btn_square > btn_square) {

	};
	b_btn_square = btn_square;


	if(ps3dat->button[PAD_KEY_TRIANGLE]) btn_tri++;
	if(!ps3dat->button[PAD_KEY_TRIANGLE]) btn_tri = 0;
	if(b_btn_tri > btn_tri) {
		
		megaPhoneLock = 1 - megaPhoneLock;
		
		if(megaPhoneLock) {
			setPCA9685Duty(fds ,  8 , -40);
			setPCA9685Duty(fds , 12 , -60);
		} else {
			setPCA9685Duty(fds ,  8 , +20);
			setPCA9685Duty(fds , 12 ,   0);
		};
	
	};
	b_btn_tri = btn_tri;


	if(ps3dat->button[PAD_KEY_CROSS]) btn_cross++;
	if(!ps3dat->button[PAD_KEY_CROSS]) btn_cross = 0;
	if(b_btn_cross > btn_cross) {

	};
	b_btn_cross = btn_cross;

	if(ps3dat->button[PAD_KEY_CIRCLE]) {
		setPCA9685Duty(fds , 12 , -60);
	};

	if(ps3dat->button[PAD_KEY_CROSS]) {
		setPCA9685Duty(fds , 0 ,  0);
		setPCA9685Duty(fds , 1 ,  0);
		setPCA9685Duty(fds , 2 ,  0);
		setPCA9685Duty(fds , 3 ,  0);
		setPCA9685Duty(fds , 4 ,  0);
		setPCA9685Duty(fds , 5 ,  0);
		setPCA9685Duty(fds , 6 ,  +20);
		setPCA9685Duty(fds , 7 ,  0);
		setPCA9685Duty(fds , 9 ,  0);
		setPCA9685Duty(fds , 10 ,  0);
		setPCA9685Duty(fds , 11 ,  0);
		setPCA9685Duty(fds , 12 ,  0);

	};

	
	if(ps3dat->button[PAD_KEY_CIRCLE]) btn_circle++;
	if(!ps3dat->button[PAD_KEY_CIRCLE]) btn_circle = 0;
	if(b_btn_circle > btn_circle) {

	};
	b_btn_circle = btn_circle;

	if(ps3dat->button[PAD_KEY_UP]) {
		 if(t_mode == 6) setPCA9685Duty(fds , t_mode ,  +75);
		 else setPCA9685Duty(fds , t_mode ,  +60);
	}
	else if(ps3dat->button[PAD_KEY_DOWN]) {
		 if(t_mode != 6) setPCA9685Duty(fds , t_mode ,  -60);
	}
	else { 
		if(t_mode == 6) setPCA9685Duty(fds , t_mode ,  +20);
		else setPCA9685Duty(fds , t_mode ,  0);
	};

	
	if((ps3dat->button[PAD_KEY_UP])&&(ps3dat->button[PAD_KEY_L2])) btn_upd++;
	if((!ps3dat->button[PAD_KEY_UP])&&(ps3dat->button[PAD_KEY_L2])) btn_upd = 0;
	if(b_btn_upd > btn_upd) {


	};

	if((ps3dat->button[PAD_KEY_DOWN])&&(ps3dat->button[PAD_KEY_L2])) btn_downd++;
	if((!ps3dat->button[PAD_KEY_DOWN])&&(ps3dat->button[PAD_KEY_L2])) btn_downd = 0;
	if(b_btn_downd > btn_downd) {


	};

		if((b_btn_upd > btn_upd)||(b_btn_downd > btn_downd)) {
			
			
		};

	b_btn_upd = btn_upd;
	b_btn_downd = btn_downd;
	

	if(ps3dat->button[PAD_KEY_LEFT]) btn_left++;
	if(!ps3dat->button[PAD_KEY_LEFT]) btn_left = 0;
	if(b_btn_left > btn_left) {
		
		t_mode--; if (t_mode < 0) t_mode = 0;
		if(t_mode == 0 ) system("mpg123 /home/pi/Music/servo-0.mp3 &");
		if(t_mode == 1 ) system("mpg123 /home/pi/Music/servo-1.mp3 &");
		if(t_mode == 2 ) system("mpg123 /home/pi/Music/servo-2.mp3 &");
		if(t_mode == 3 ) system("mpg123 /home/pi/Music/servo-3.mp3 &");
		if(t_mode == 4 ) system("mpg123 /home/pi/Music/servo-4.mp3 &");
		if(t_mode == 5 ) system("mpg123 /home/pi/Music/servo-5.mp3 &");
		if(t_mode == 6 ) system("mpg123 /home/pi/Music/servo-6.mp3 &");
		if(t_mode == 7 ) system("mpg123 /home/pi/Music/servo-7.mp3 &");
		if(t_mode == 8 ) system("mpg123 /home/pi/Music/servo-8.mp3 &");
		if(t_mode == 9 ) system("mpg123 /home/pi/Music/servo-9.mp3 &");
		if(t_mode == 10 ) system("mpg123 /home/pi/Music/servo-10.mp3 &");
		if(t_mode == 11 ) system("mpg123 /home/pi/Music/servo-11.mp3 &");
		if(t_mode == 12 ) system("mpg123 /home/pi/Music/servo-12.mp3 &");
	};

	if(ps3dat->button[PAD_KEY_RIGHT]) btn_right++;
	if(!ps3dat->button[PAD_KEY_RIGHT]) btn_right = 0;
	if(b_btn_right > btn_right) {

		t_mode++; if (t_mode > 12) t_mode = 12;
		if(t_mode == 0 ) system("mpg123 /home/pi/Music/servo-0.mp3 &");
		if(t_mode == 1 ) system("mpg123 /home/pi/Music/servo-1.mp3 &");
		if(t_mode == 2 ) system("mpg123 /home/pi/Music/servo-2.mp3 &");
		if(t_mode == 3 ) system("mpg123 /home/pi/Music/servo-3.mp3 &");
		if(t_mode == 4 ) system("mpg123 /home/pi/Music/servo-4.mp3 &");
		if(t_mode == 5 ) system("mpg123 /home/pi/Music/servo-5.mp3 &");
		if(t_mode == 6 ) system("mpg123 /home/pi/Music/servo-6.mp3 &");
		if(t_mode == 7 ) system("mpg123 /home/pi/Music/servo-7.mp3 &");
		if(t_mode == 8 ) system("mpg123 /home/pi/Music/servo-8.mp3 &");
		if(t_mode == 9 ) system("mpg123 /home/pi/Music/servo-9.mp3 &");
		if(t_mode == 10 ) system("mpg123 /home/pi/Music/servo-10.mp3 &");
		if(t_mode == 11 ) system("mpg123 /home/pi/Music/servo-11.mp3 &");
		if(t_mode == 12 ) system("mpg123 /home/pi/Music/servo-12.mp3 &");
	};

		if((b_btn_left > btn_left)||(b_btn_right > btn_right)) {
			
			
		};

	b_btn_left = btn_left;
	b_btn_right = btn_right;


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

	fds = wiringPiI2CSetup(0x41);	// PCA9685
	resetPCA9685(fds);
	setPCA9685Freq(fds,50);
	system("mpg123 /home/pi/Music/Main_system_startup.mp3");
	delay(200);
	system("mpg123 /home/pi/Music/Press_the_PS_button.mp3");
	

	if(!(ps3c_init(&ps3dat, df))) {
		
		setPCA9685Duty(fds , 0 ,  0);
		setPCA9685Duty(fds , 1 ,  0);
		setPCA9685Duty(fds , 2 ,  0);
		setPCA9685Duty(fds , 3 ,  0);
		setPCA9685Duty(fds , 4 ,  0);
		setPCA9685Duty(fds , 5 ,  0);
		setPCA9685Duty(fds , 6 ,  +20);
		setPCA9685Duty(fds , 7 ,  0);
		setPCA9685Duty(fds , 9 ,  0);
		setPCA9685Duty(fds , 10 ,  0);
		setPCA9685Duty(fds , 11 ,  0);
		setPCA9685Duty(fds , 12 ,  0);

		if(megaPhoneLock) {
			setPCA9685Duty(fds ,  8 , -40);
			setPCA9685Duty(fds , 12 , -60);
		} else {
			setPCA9685Duty(fds ,  8 , +20);
			setPCA9685Duty(fds , 12 ,   0);
		};

		do {
			if (ps3c_test(&ps3dat) < 0) break;
		} while (!(ps3c_input(&ps3dat)));

		ps3c_exit(&ps3dat);
	}
}

