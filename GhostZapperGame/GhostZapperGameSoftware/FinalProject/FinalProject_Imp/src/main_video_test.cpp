/*****************************************************************//**
 * @file main_video_test.cpp
 *
 * @brief Basic test of 4 basic i/o cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

//#define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "xadc_core.h"
#include "sseg_core.h"
#include "spi_core.h"
#include "i2c_core.h"
#include "ps2_core.h"
#include "ddfs_core.h"
#include "adsr_core.h"
#include "vga_core.h"

static int Xmouse1 = 0, Ymouse1 = 0, Xmouse2 = 0, Ymouse2 = 0, GX1 = 0, GY1 = 0, GX2 = 0, GY2 = 0;

void test_start(GpoCore *led_p) {
   int i;

   for (i = 0; i < 20; i++) {
      led_p->write(0xff00);
      sleep_ms(50);
      led_p->write(0x0000);
      sleep_ms(50);
   }
}

/**
 * check bar generator core
 * @param bar_p pointer to Gpv instance
 */
void bar_check(GpvCore *bar_p) {
   bar_p->bypass(0);
   sleep_ms(3000);
}

/**
 * check color-to-grayscale core
 * @param gray_p pointer to Gpv instance
 */
void gray_check(GpvCore *gray_p) {
   gray_p->bypass(0);
   sleep_ms(3000);
   gray_p->bypass(1);
}

/**
 * check osd core
 * @param osd_p pointer to osd instance
 */
void osd_check(OsdCore *osd_p) {
   osd_p->set_color(0x0f0, 0x001); // dark gray/green
   osd_p->bypass(0);
   osd_p->clr_screen();
   for (int i = 0; i < 64; i++) {
      osd_p->wr_char(8 + i, 20, i);
      osd_p->wr_char(8 + i, 21, 64 + i, 1);
      sleep_ms(100);
   }
   sleep_ms(3000);
}

/**
 * test frame buffer core
 * @param frame_p pointer to frame buffer instance
 */
void frame_check(FrameCore *frame_p) {
   int x, y, color;

   frame_p->bypass(0);
   for (int i = 0; i < 10; i++) {
      frame_p->clr_screen(0x008);  // dark green
      for (int j = 0; j < 20; j++) {
         x = rand() % 640;
         y = rand() % 480;
         color = rand() % 512;
         frame_p->plot_line(400, 200, x, y, color);
      }
      sleep_ms(300);
   }
   sleep_ms(3000);
}

/**
 * test mouse sprite
 * @param mouse_p pointer to mouse sprite instance
 */
void mouse_check(SpriteCore *mouse_p) {
   int x, y;

   mouse_p->bypass(0);
   // clear top and bottom lines
   for (int i = 0; i < 32; i++) {
      mouse_p->wr_mem(i, 0);
      mouse_p->wr_mem(31 * 32 + i, 0);
   }

   // slowly move mouse pointer
   x = 0;
   y = 0;
   for (int i = 0; i < 80; i++) {
      mouse_p->move_xy(x, y);
      sleep_ms(50);
      x = x + 4;
      y = y + 3;
   }
   sleep_ms(3000);
   // load top and bottom rows
   for (int i = 0; i < 32; i++) {
      sleep_ms(20);
      mouse_p->wr_mem(i, 0x00f);
      mouse_p->wr_mem(31 * 32 + i, 0xf00);
   }
   sleep_ms(3000);
}

/**
 * test ghost sprite
 * @param ghost_p pointer to ghost sprite instance
 */
void ghost_check(SpriteCore *ghost_p) {
   int x, y;

   // slowly move mouse pointer
   ghost_p->bypass(0);
   ghost_p->wr_ctrl(0x1c);  //animation; blue ghost
   x = 0;
   y = 100;
   for (int i = 0; i < 156; i++) {
      ghost_p->move_xy(x, y);
      sleep_ms(100);
      x = x + 4;
      if (i == 80) {
         // change to red ghost half way
         ghost_p->wr_ctrl(0x04);
      }
   }
   sleep_ms(3000);
}

void ddfs_check(DdfsCore *ddfs_p, GpoCore *led_p) {
   int i, j;
   float env;

   //vol = (float)sw.read_pin()/(float)(1<<16),
   ddfs_p->set_env_source(0);  // select envelop source
   ddfs_p->set_env(0.0);   // set volume
   sleep_ms(500);
   ddfs_p->set_env(1.0);   // set volume
   ddfs_p->set_carrier_freq(262);
   sleep_ms(2000);
   ddfs_p->set_env(0.0);   // set volume
   sleep_ms(2000);
   // volume control (attenuation)
   ddfs_p->set_env(0.0);   // set volume
   env = 1.0;
   for (i = 0; i < 1000; i++) {
      ddfs_p->set_env(env);
      sleep_ms(10);
      env = env / 1.0109; //1.0109**1024=2**16
   }
   // frequency modulation 635-912 800 - 2000 siren sound
   ddfs_p->set_env(1.0);   // set volume
   ddfs_p->set_carrier_freq(635);
   for (i = 0; i < 5; i++) {               // 10 cycles
      for (j = 0; j < 30; j++) {           // sweep 30 steps
         ddfs_p->set_offset_freq(j * 10);  // 10 Hz increment
         sleep_ms(25);
      } // end j loop
   } // end i loop
   ddfs_p->set_offset_freq(0);
   ddfs_p->set_env(0.0);   // set volume
   sleep_ms(1000);
}

void init(Ps2Core *ps2_p){
	ps2_p -> init();
}

int SinglePlayerMouse(SpriteCore *mouse_p, Ps2Core *ps2_p){
	int lb, rb, X, Y;
	//mouse_p->bypass(0);
	   // clear top and bottom lines


		//uart.disp("nope");
		//uart.disp("\n\r");
		//Xmouse = Xmouse;
		//Ymouse = Ymouse;
		//sleep_ms(20);
	if (ps2_p->get_mouse_activity(&lb, &rb, &X, &Y) == 1){

		ps2_p->get_mouse_activity(&lb, &rb, &X, &Y);
		Xmouse1 += X;
		Ymouse1 -= Y;
		if(Xmouse1 >= 640)
			Xmouse1 = 640;
		if(Xmouse1 <= 0)
			Xmouse1 = 0;
		if(Ymouse1 >= 480)
			Ymouse1 = 480;
		if(Ymouse1 <= 0)
			Ymouse1 = 0;
		mouse_p->move_xy(Xmouse1, Ymouse1);
		/*uart.disp(Xmouse);
		uart.disp("\n\r");
		uart.disp(Ymouse);
		uart.disp("\n\r");
		uart.disp("\n\r");*/
	}
return lb;
}


void mouse_adc(XadcCore *adc_p, SpriteCore *mouse_p, int mouseSel) { //0 or 1
	int Xmouse, Ymouse;
	//pricey but pretty
	//double Xadc = adc_p -> read_adc_in(0);
	//double Yadc = adc_p -> read_adc_in(2);
	//int Xm = Xadc*640; //  //640
	//int Ym = Yadc*480; // //480

	//efficient but ugly
	int Xadc = adc_p -> read_raw(mouseSel);
	int Yadc = adc_p -> read_raw(mouseSel+1);
	Xmouse = ((Xadc>>7) + (Xadc>>9))&0xfff8; //
	Ymouse = ((Yadc>>8) + (Yadc>>9) + (Yadc>>10) + (Yadc>>11))&0xfff8; //

	mouse_p->move_xy(Xmouse, Ymouse);

	if(mouseSel == 0){
		Xmouse1 = Xmouse;
		Ymouse1 = Ymouse;
	}

	if(mouseSel == 1){
		Xmouse2 = Xmouse;
		Ymouse2 = Ymouse;
	}
	//sleep_ms(20);
	/*uart.disp(Xmouse);
	uart.disp("\n\r");
	uart.disp(Ymouse);
	uart.disp("\n\r");
	uart.disp("\n\r");*/
   }


void ps2_check(Ps2Core *ps2_p) {
   int id;
   int lbtn, rbtn, xmov, ymov;
   char ch;
   unsigned long last;

   uart.disp("\n\rPS2 device (1-keyboard / 2-mouse): ");
   id = ps2_p->init();
   uart.disp(id);
   uart.disp("\n\r");
   last = now_ms();
   do {
      if (id == 2) {  // mouse
         if (ps2_p->get_mouse_activity(&lbtn, &rbtn, &xmov, &ymov)) {
            uart.disp("[");
            uart.disp(lbtn);
            uart.disp(", ");
            uart.disp(rbtn);
            uart.disp(", ");
            uart.disp(xmov);
            uart.disp(", ");
            uart.disp(ymov);
            uart.disp("] \r\n");
            last = now_ms();

         }   // end get_mouse_activitiy()
      } else {
         if (ps2_p->get_kb_ch(&ch)) {
            uart.disp(ch);
            uart.disp(" ");
            last = now_ms();
         } // end get_kb_ch()
      }  // end id==2
   } while (now_ms() - last < 5000);
   uart.disp("\n\rExit PS2 test \n\r");

}

int check(uint8_t controller){ //checks if specified controller hit a ghost
	int Xmouse, Ymouse;

	switch(controller){
	case(0):Xmouse = Xmouse1;
			Ymouse = Ymouse1;
			break;
	case(1):Xmouse = Xmouse2;
			Ymouse = Ymouse2;
			break;
	default:Xmouse = 0;
			Ymouse = 0;
			break;
	}

	if((((GX1 - Xmouse > 0) && (GX1 - Xmouse <= 10))||((Xmouse - GX1 > 0) && (Xmouse - GX1 <= 10)))){ //checks ghost1's X axis
		if((((GY1 - Ymouse > 0) && (GY1 - Ymouse <= 10))||((Ymouse - GY1 > 0) && (Ymouse - GY1 <= 10)))){ //checks ghost1's Y axis
			return 5; //[ghost died] [ghost1 alive] [ghost0 dead] = 101
		}
		if((((GY2 - Ymouse > 0) && (GY2 - Ymouse <= 10))||((Ymouse - GY2 > 0) && (Ymouse - GY2 <= 10)))){ //checks ghost2's Y axis in case both ghosts were roughly in the same area
			return 6; //[ghost died] [ghost1 dead] [ghost0 alive] = 110
		}
	}

	if((((GX2 - Xmouse > 0) && (GX2 - Xmouse <= 10))||((Xmouse - GX2 > 0) && (Xmouse - GX2 <= 10)))){
		if((((GY1 - Ymouse > 0) && (GY1 - Ymouse <= 10))||((Ymouse - GY1 > 0) && (Ymouse - GY1 <= 10)))){
			return 5; //[ghost died] [ghost1 alive] [ghost0 dead] = 101
		}
		if((((GY2 - Ymouse > 0) && (GY2 - Ymouse <= 10))||((Ymouse - GY2 > 0) && (Ymouse - GY2 <= 10)))){
			return 6; //[ghost died] [ghost1 dead] [ghost0 alive] = 110
		}
	}

	else{ //no ghost dead
		return 0; //0x [no ghost died] [ghost1 alive] [ghost0 alive] = 0
	}
}

void ghostGEN (){ //takes ghost gen and adds appropriate sprites on screen, saves movement values to

}

void game(GpiCore *sw_p, DebounceCore *db_p, SpriteCore *mouse_p, Ps2Core *ps2_p, XadcCore *adc_p, SpriteCore *mouse_p,){
	int ghostSTAT = 3, score0 = 0, score1 = 0; //holds life of two ghosts in binary (0 if alive, 1 if Dead)
	//initiate timer core
	while(/*timer isn't done*/){
		//start timer
		if( sw_p->read() == 0x0001){ //checks if singleplayer mouse
			SinglePlayerMouse(mouse_p, ps2_p);
			if(SinglePlayerMouse(mouse_p, ps2_p) == 1){		//if player1 pressed button
							ghostSTAT = check(0);
							score0 += ghostSTAT >> 2;
						}
		}

		else{						//else two player controller input
			   mouse_adc(&adc, &mouse, 0);
			   mouse_adc(&adc, &mouse, 1);
			if(button1 == 1){		//if player1 pressed button
				ghostSTAT = check(0);
				score0 += ghostSTAT >> 2;
			}
			if(button2 == 1){		//if player2 pressed button
				ghostSTAT = check(1);
				score1 += ghostSTAT >> 2;
			}
		}
	}
}


// external core instantiation
Ps2Core ps2(get_slot_addr(BRIDGE_BASE, S11_PS2));
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
FrameCore frame(FRAME_BASE);
GpvCore bar(get_sprite_addr(BRIDGE_BASE, V7_BAR));
GpvCore gray(get_sprite_addr(BRIDGE_BASE, V6_GRAY));
SpriteCore ghost(get_sprite_addr(BRIDGE_BASE, V3_GHOST), 1024);
SpriteCore mouse(get_sprite_addr(BRIDGE_BASE, V1_MOUSE), 1024);
OsdCore osd(get_sprite_addr(BRIDGE_BASE, V2_OSD));
XadcCore adc(get_slot_addr(BRIDGE_BASE, S5_XDAC));

int main() {
	init(&ps2);
   while (1) {
      /*test_start(&led);
      // bypass all cores; blue screen
      frame.bypass(1);
      bar.bypass(1);
      gray.bypass(1);
      ghost.bypass(1);
      osd.bypass(1);
      mouse.bypass(1);
      sleep_ms(3000);

      // enable cores one by one
      frame_check(&frame);
      bar_check(&bar);
      gray_check(&gray);
      ghost_check(&ghost);
      osd_check(&osd);
      mouse_check(&mouse);
      while (sw.read(0)) {
         // test composition with different overlays if sw(0) is 1
         mouse.bypass(sw.read(1));
         osd.bypass(sw.read(2));
         ghost.bypass(sw.read(3));
         gray.bypass(sw.read(6));
         bar.bypass(sw.read(7));
         frame.bypass(sw.read(8));
         // set osd background color to transparent
         osd.set_color(0x0f0, sw.read(9));
         // set color/animation of ghost sprite
         ghost.wr_ctrl(sw.read() >> 11);
      } //while*/
	   //ps2_check(&ps2);
	   //lab13(&mouse, &ps2);

   } // while
} //main
