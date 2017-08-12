// cc -o self self.c -lwiringPi -lm
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

#include <wiringPi.h>
#include <softPwm.h>
#include <softTone.h>

#include "controller.h"
#include <math.h>


struct ps3ctls {
    int fd;
    unsigned char nr_buttons;   // Max number of Buttons
    unsigned char nr_sticks;    // Max number of Sticks
    short *button;          // button[nr_buttons]
    short *stick;           // stick[nr_sticks]
};


int f = 1; // モニタに戻るフラグ
int b_btn_start = 0;
int btn_start   = 0;


int write_file(char fnp[256],long int d) {

	FILE *fp;
	char fn[256]="/tmp/";
	char cn[256]="chmod 666 ";

	strcat(fn,fnp);
	strcat(fn,".txt");
	strcat(cn,fn);

	if((fp=fopen(fn,"r+"))==NULL) {
		fp=fopen(fn,"w+");
		system(cn);
	};

	fprintf(fp,"%8d",d);
	fclose(fp);
}


long int read_file(char fnp[256]) {

	FILE *fp;
	char fn[256]="/tmp/";
	long int d;

	strcat(fn,fnp);
	strcat(fn,".txt");

	if((fp=fopen(fn,"r"))!=NULL) {
		fscanf(fp,"%8d",&d);
		fclose(fp);
	} else d=0;

	return d;
}


int automatic() {

	int sensor;

    //system("python /home/pi/r2017/simplebeep.py");
    digitalWrite(23,0);	// センササーボ 0:収納
	if(digitalRead(15)==0) {
		delay(500);
        if(digitalRead(15)==0) {  // 長押しの場合は、startxとタイプして、プログラム修正できるようにする。
            softPwmWrite( 1,0);  // とりあえず、大回転モータを停止しとく
            f = 0; // モニタに戻る
        };
		if(digitalRead(15)!=0) {  // 瞬間押しの場合は、自動モードに入る。

			softPwmWrite( 1,30);	// １回転目
			delay(1000);
            softPwmWrite( 1,15);
			while(digitalRead(4));
			softPwmWrite( 1,0);
			delay(500);

			softPwmWrite( 1,30);	// ２回転目
			delay(1000);
            softPwmWrite( 1,15);
			while(digitalRead(4));
			softPwmWrite( 1,0);
			delay(500);

			digitalWrite(23,1);	// センササーボ 1:出し
			delay(500);

			softPwmWrite( 5,50);	// ホイールで前進
			softPwmWrite(27,50);
			delay(500);
			softPwmWrite( 5,0);
			softPwmWrite(27,0);
			delay(500);

			while(digitalRead(15)==0);   // スイッチ待ち
			delay(500);
			while(digitalRead(15)!=0);

			softPwmWrite( 6,50);	// ホイールで後進
			softPwmWrite(26,50);
			delay(500);
			softPwmWrite( 6,0);
			softPwmWrite(26,0);
			delay(500);

			digitalWrite(23,0);	// センササーボ 0:収納
		}; // if(digitalRead(15)!=0)
	}; // if(digitalRead(15)==0)
}	// automatic()


int automatic_test() {

    int sensor;

    digitalWrite(23,0); // センササーボ 0:収納

    delay(500);
    softPwmWrite( 6,50); // 左回転
    softPwmWrite(27,50);
    while(read_file("cntWheel")<=50);
    softPwmWrite( 6,0);
    softPwmWrite(27,0);
    write_file("cntWheel",0);
    delay(500);

    digitalWrite(23,0); // センササーボ 0:収納

}   // automatic_test()

int automatic_test2() {

    int sensor;

    digitalWrite(23,1); // センササーボ 1:出し

    if(digitalRead(15)==0) {
        delay(500);
        if(digitalRead(15)==0) {  // 長押しの場合は、startxとタイプして、プログラム修正できるようにする。
            softPwmWrite( 1,0);  // とりあえず、大回転モータを停止しとく
            f = 0; // モニタに戻る
        };
        if(digitalRead(15)!=0) {  // 瞬間押しの場合は、自動モードに入る。

            write_file("cntWheel",0);

            while(read_file("cntWheel")<=100) { // 100カウントで自動停止

                // センサ 白:1 黒:0 左から右
                sensor = digitalRead(7) * 64 + digitalRead(0) * 32 + digitalRead(2) * 16 + digitalRead(3) * 8 +
                                                digitalRead(12) * 4 + digitalRead(13) * 2 + digitalRead(14);

                softPwmWrite( 5,20); // 左車輪前
                softPwmWrite(27,20); // 右車輪前

                if(sensor==001) { softPwmWrite( 5,20); softPwmWrite(27,10);}; // WWW W WWB
                if(sensor==002) { softPwmWrite( 5,20); softPwmWrite(27,10);}; // WWW W WBW
                if(sensor==003) { softPwmWrite( 5,20); softPwmWrite(27,10);}; // WW W W W B B
                if(sensor==004) { softPwmWrite( 5,20); softPwmWrite(27,10);}; // WW W W B W W
                if(sensor==005) { softPwmWrite( 5,20); softPwmWrite(27,10);}; // WW W W B W B
                if(sensor==006) { softPwmWrite( 5,20); softPwmWrite(27,10);}; // WW W W B B W
                if(sensor==007) { softPwmWrite( 5,20); softPwmWrite(27,10);}; // W  W   W   W   B   B   B
                if(sensor==010) { softPwmWrite( 5,20); softPwmWrite(27,10);}; // W  W   W   B   W   B   W
                if(sensor==011) { softPwmWrite( 5,20); softPwmWrite(27,10);}; // W  W   W   B   W   B   B
                if(sensor==013) { softPwmWrite( 5,20); softPwmWrite(27,10);}; // W  W   W   B   B   W   B
                if(sensor==014) { softPwmWrite( 5,20); softPwmWrite(27,10);}; // W  W   W   B   B   B   W
                if(sensor==016) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // W  W   B   W   W   W   W
                if(sensor==017) { softPwmWrite( 5,20); softPwmWrite(27,10);}; // W  W   B   W   W   W   B
                if(sensor==019) { softPwmWrite( 5,20); softPwmWrite(27,10);}; // W  W   B   W   W   B   B
                if(sensor==024) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // W  W   B   B   W   W   W
                if(sensor==025) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // W  W   B   B   W   W   B
                if(sensor==026) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // W  W   B   B   W   B   W
                if(sensor==032) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // W  B   W   W   W   W   W
                if(sensor==035) { softPwmWrite( 5,20); softPwmWrite(27,10);}; // W  B   W   W   W   B   B
                if(sensor==038) { softPwmWrite( 5,20); softPwmWrite(27,10);}; // W  B   W   W   B   B   W
                if(sensor==040) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // W  B   W   B   W   W   W
                if(sensor==044) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // W  B   W   B   B   W   W
                if(sensor==048) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // W  B   B   W   W   W   W
                if(sensor==049) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // W  B   B   W   W   W   B
                if(sensor==050) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // W  B   B   W   W   B   W
                if(sensor==052) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // W  B   B   W   B   W   W
                if(sensor==056) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // W  B   B   B   W   W   W
                if(sensor==057) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // W  B   B   B   W   W   B
                if(sensor==064) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // B  W   W   W   W   W   W
                if(sensor==068) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // B  W   W   W   B   W   W
                if(sensor==071) { softPwmWrite( 5,20); softPwmWrite(27,10);}; // B  W   W   W   B   B   B
                if(sensor==072) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // B  W   W   B   W   W   W
                if(sensor==078) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // B  W   W   B   B   B   W
                if(sensor==079) { softPwmWrite( 5,20); softPwmWrite(27,10);}; // B  W   W   B   B   B   B
                if(sensor==080) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // B  W   B   W   W   W   W
                if(sensor==088) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // B  W   B   B   W   W   W
                if(sensor==096) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // B  B   W   W   W   W   W
                if(sensor==097) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // B  B   W   W   W   W   B
                if(sensor==098) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // B  B   W   W   W   B   W
                if(sensor==100) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // B  B   W   W   B   W   W
                if(sensor==104) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // B  B   W   B   W   W   W
                if(sensor==112) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // B  B   B   W   W   W   W
                if(sensor==114) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // B  B   B   W   W   B   W
                if(sensor==116) { softPwmWrite( 5,10); softPwmWrite(27,20);}; // B  B   B   W   B   W   W
            }; // while(read_file("cntWheel")

            write_file("cntWheel",0);
            delay(500);

            digitalWrite(23,0); // センササーボ 0:収納

        }; // if(digitalRead(15)!=0)

    }; // if(digitalRead(15)==0)

}   // automatic_test2()


int ps3c_test(struct ps3ctls *ps3dat) {

    int i;
    unsigned char nr_btn = ps3dat->nr_buttons;
    unsigned char nr_stk = ps3dat->nr_sticks;
    int xx,yy,ph,x,y,z,p,c1,c2,c3,c4,v1,v2,ww;

    printf(" s7=%4d ",digitalRead(7));   // 左床センサ 白:1 黒:0
    printf(" s0=%4d ",digitalRead(0));
    printf(" s2=%4d ",digitalRead(2));
    printf(" s3=%4d ",digitalRead(3));   // 中心床センサ
    printf(" s12=%3d ",digitalRead(12));
    printf(" s13=%3d ",digitalRead(13));
    printf(" s14=%3d ",digitalRead(14));   // 右床センサ
    printf(" s15=%3d ",digitalRead(15));   // タクトスイッチ 通常:1 検出時:0
    printf(" s16=%3d ",digitalRead(16));   // エンコーダ 通常:1 回転時:0
    printf(" s1=%4d ",digitalRead(4));   // 大回転センサ 通常:1 検出時:0
	printf(" enc=%4d ",read_file("cntWheel"));   // エンコーダ
    printf("\n");

//    automatic();

//	if((digitalRead(4)!=rotation)&&(digitalRead(4)==0)) system("python /home/pi/r2017/simplebeep.py");
//	rotation = digitalRead(4);

    if( ps3dat->button[PAD_KEY_LEFT  ])	{
		softPwmWrite( 5,20); // 左車輪前
	}
	else if( ps3dat->button[PAD_KEY_RIGHT ]) {
		softPwmWrite(27,20); // 右車輪前
	}
	else if( ps3dat->button[PAD_KEY_UP]) {
		softPwmWrite( 5,20); // 左右車輪前
		softPwmWrite(27,20);
	}
	else if( ps3dat->button[PAD_KEY_DOWN]) {
		softPwmWrite( 6,20); // 左右車輪後
		softPwmWrite(26,20);
	}
	else if( ps3dat->button[PAD_KEY_CIRCLE]) {
		softPwmWrite( 5,50); // 右回転
		softPwmWrite(26,50);
	}
	else if( ps3dat->button[PAD_KEY_SQUARE]) {
		softPwmWrite( 6,50); // 左回転
		softPwmWrite(27,50);
	}
	else if( ps3dat->button[PAD_KEY_TRIANGLE]) {
		digitalWrite(23,1); // センサ出し
	}
	else if( ps3dat->button[PAD_KEY_CROSS]) {
		digitalWrite(23,0); // センサ収納
	}else {
		softPwmWrite( 5,0); // 停止
		softPwmWrite( 6,0);
		softPwmWrite(27,0);
		softPwmWrite(26,0);
	};

 //   if(ps3dat->button[PAD_KEY_SELECT]   ) digitalWrite(25,1); else digitalWrite(25,0); // Lamp test

	if(ps3dat->button[PAD_KEY_START]) btn_start++;
	if(!ps3dat->button[PAD_KEY_START]) btn_start = 0;
	if(b_btn_start > btn_start) {
		automatic_test2();
    };
    b_btn_start = btn_start;

    if(digitalRead(15)==0) return -1; // end of program


    return 0;
}


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

    if(digitalRead(15)==0) return -1; // end of program

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
//  printf("%d %d\n",nr_btn,nr_stk);

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
    long int i,j;

    wiringPiSetup();
    softPwmCreate( 5,0,20); // motor-1 20ms   // 左モータ
    softPwmCreate( 6,0,20); // motor-1 20ms
    softPwmCreate(27,0,20); // motor-2 20ms   // 右モータ
    softPwmCreate(26,0,20); // motor-2 20ms
    softPwmCreate( 1,0,20); // motor-3 20ms   // 大回転モータ

    pinMode(23,OUTPUT);	// センササーボ 0:収納 1:出し
    pinMode(25,OUTPUT);	// ブザーを止めてLEDにする
    digitalWrite(25,1); // 動作中LED点灯

    pinMode(7,INPUT);   // 左床センサ
    pinMode(0,INPUT);
    pinMode(2,INPUT);
    pinMode(3,INPUT);   // 中心床センサ
    pinMode(12,INPUT);
    pinMode(13,INPUT);
    pinMode(14,INPUT);   // 右床センサ

    pinMode(15,INPUT);	// タクトスイッチ
	pinMode(16,INPUT);	// エンコーダ
	pinMode(4,INPUT);	// 大回転センサ

	write_file("cntWheel"		,0 );
	digitalWrite(25,1); // Lamp ON

//	system("mpg123 /home/pi/Music/move_it.mp3 &");
//	system("python /home/pi/r2017/simplebeep.py");

    if(!(ps3c_init(&ps3dat, df))) {

		f = 0; // モニタに戻らないようにしとく

        do {
            if (ps3c_test(&ps3dat) < 0) break;
        } while (!(ps3c_input(&ps3dat)));

        ps3c_exit(&ps3dat);

    } else // if(!(ps3c_init(&ps3dat, df)))
        while(f) automatic_test2();

    digitalWrite(25,0); // 動作中LED消灯
}
