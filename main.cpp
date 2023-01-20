

#include "Arduino.h"
#include "hd44780/HD44780.hpp"
#include "libADC/libADC.hpp"
#include "UART/uart.hpp";




boolean dinoOnGround = true;
 //declare a boolean to check if the dino is in the ground


int buttonState = 0;
 // a token variable to check the state of the button

int highScore = 0;

char buff[8];


#define btnUP     1 // button to jump
#define btnSELECT 4 // button to start the game
#define btnNONE   5


boolean playState = false;
 //the game starts only when this becomes true
 // This is a common method used in coding games

int dist = 0;
int distTwo = 0;
int temp=0;
 // variables to set the distance between the trees

int score = 0;


//bitmap array for the dino character
byte dino [8]
{ B00000,
  B00111,
  B00101,
  B10111,
  B11100,
  B11111,
  B01101,
  B01100,
};

//character for the tree
byte tree [8]
{
  B00011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11111,
  B01110,
  B01110
};
//character for the bird
byte bird[8] = {
  0b00000,
  0b00100,
  0b01100,
  0b11110,
  0b00111,
  0b00110,
  0b00100,
  0b00000
};
int raw= 0;
int read_LCD_buttons(){
	while (1){
			ADCSRA |= (1<<ADSC);
			while(ADCSRA & (1<<ADSC));
			raw = ADC;
			if ((100 < raw) && (raw < 250)) {
				return btnUP;
			}
			if ((600 < raw)&& (900 >raw) ) {
				return btnSELECT;
			}
			return btnNONE;
	}
}
void CreateCustomCharacter (unsigned char *Pattern, const char Location){
	LCD_WriteCommand(0x40+(Location*8));
	for (int i=0; i<8; i++)
	LCD_WriteData(Pattern[i]);
}

int main(void){
	LCD_Initalize();
	ADC_Init();
	sei();
	uart_init(9600,0);
	const uint8_t start[] = "\n\rthe game has started\n\r";
	const uint8_t finish[] = "the game has finished\n\r";
	const uint8_t sendScore[] = "your currently higher score is ";
	CreateCustomCharacter(dino, 7);
	CreateCustomCharacter(tree, 6);
	CreateCustomCharacter(bird, 8);
	int counter=0;
	int speed =200;
	while(1) {
		if(!playState){
			LCD_GoTo(0, 0);
			LCD_WriteText("Press select");
			LCD_GoTo(0, 1);
			LCD_WriteText("to start: ");
	}
		if(read_LCD_buttons()== btnSELECT){ //Read the button
			  playState = true;
			  uart_send_string(start);

		}


		if(playState){

			LCD_Clear();
			dist = random(4,9);
			distTwo = random(4,9);
			temp=random(2,10);

			for(int i=16; i>=-(dist+distTwo); i--){
				int state= read_LCD_buttons();
				LCD_GoTo(13,0);
				sprintf(buff,"%u",score);
				LCD_WriteText(buff);
				score++;

				if(state == btnUP){
					LCD_GoTo(1,0); // row and set dinoOnGround to false
					LCD_WriteData(7);
					LCD_GoTo(1,1);
					LCD_WriteText(" ");
					dinoOnGround = false;
				}else{
					LCD_GoTo(1,1);   // print the dino on the lower row
					LCD_WriteData(7);              // and set dinoOnGround to true
					LCD_GoTo(1,0);
					LCD_WriteText(" ");
					dinoOnGround = true;
				}
					  LCD_GoTo(i,1);
					  LCD_WriteData(6);
					  LCD_GoTo(i+1,1);
					  LCD_WriteText(" ");

					  if((temp!=dist)&&(temp!=distTwo)&&(temp!=distTwo+1)&&(temp!=dist+1)&&(temp!=dist-1)&&(temp!=dist-1)){
						  LCD_GoTo(i+temp,0);
						  LCD_WriteData(8);
						  LCD_GoTo(i+temp+1,0);
						  LCD_WriteText(" ");
					  }else{
						  temp=random(2,10);
					  }


				  	  LCD_GoTo(i+dist,1);
				  	  LCD_WriteData(6);
				  	  LCD_GoTo(i+dist+1,1);
				  	  LCD_WriteText(" ");


				  	  LCD_GoTo(i+dist+distTwo,1);
				  	  LCD_WriteData(6);
				  	  LCD_GoTo(i+dist+distTwo+1,1);
				  	  LCD_WriteText(" ");


				  	  _delay_ms(speed); // speed

				  	  if((i+dist+distTwo)==-1){
				  		  i=12;
				  	  }

				  	  if(((i==1||(i+dist==1)||(i+dist+distTwo==1)) && dinoOnGround)){
				  		  LCD_Clear();
				  		  LCD_WriteText("Game Over!!!");

				  		  if(score>highScore){
				  			  highScore = score;
				  		  }
				  		  	  LCD_GoTo(0,1);
				  		  	  LCD_WriteText("score: ");
				  		  	  sprintf(buff,"%u",score);
				  		  	  LCD_WriteText(buff);
				  		  	  playState = false;  //Set the playState as false so it goes to the
				  		  	  uint8_t data = 0;
				  		  	  data = uart_read();
				  		  	  uart_send_string(finish);
				  		  	  sprintf(buff,"%u",highScore);
				  		  	  uart_send_string(sendScore);
				  		  	  uart_send_string(buff);

				  		  	  _delay_ms(5000);         // start screen after the game is over
				  		  	  score = 0;

				  		  	  break;
				  	  }
				  	  if((i+temp==1) && !dinoOnGround){
				  		  LCD_Clear();
				  		  LCD_WriteText("Game Over!!!");

				  		  if(score>highScore){
				  			  highScore = score;
				  		  }
				  		  LCD_GoTo(0,1);
				  		  LCD_WriteText("score: ");
				  		  sprintf(buff,"%u",score);
				  		  LCD_WriteText(buff);
				  		  playState = false;  //Set the playState as false so it goes to the
				  		  uint8_t data = 0;
				  		  data = uart_read();
				  		  uart_send_string(finish);
				  		  sprintf(buff,"%u",highScore);
				  		  uart_send_string(sendScore);
				  		  uart_send_string(buff);
				  		  _delay_ms(5000);         // start screen after the game is over
				  		  score = 0;

				  		  break;
				  	  }

			}

		}

	}
}





