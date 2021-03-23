//https://github.com/ThomasGravekamp/Arduino-Heartbeat-LED - may try
/*

По функціоналу:
-В серце всередину вбудовано 1 або 2 вібромоторчики. Орієнтовно - 15мм довжина 5мм діаметр (точні розміри скину, пізніше дам сам мотор) - передбачити кріплення зсередини.
-В серце з тильної сторони вбудовано 4 світлодіоди по кутах - які  світять червоним на підкладку-коробочку. Діоди - в силіконовому корпусі, вставляються зсередини в отвори, повністю закриваючи їх. Посадочний діаметр - близько 8-10мм, точні розміри скину, пізніше дам сам діод.
-В передній частині коробочки під серцем (наприклад справа знизу від серця) закріплена металева антивандальна кнопка (виберу іншу форму, ніж на грамофоні - можливо з підсвіткою, для кращої ідентифікації). Діаметр посадочний - орієнтовно 20мм.
-В передній частині коробочки під серцем (наприклад зліва знизу від серця) зсередини закріплено динамік 2-3 Вт, діаметром близько 40-55 мм (передбачити отвори для проходження звуку, кріплення - на клей).  -Також можливе розміщення в нижній частині 2х плоских мініатюрних 1Вт динаміків розміром 10*15мм (передбачити отвори для звуку, кріплення - на клей).

Робочий режим:
-В пасивному режимі - діоди мигають з частотою 60 уд/сек, моторчик вібрує синхронно діодам, максимально імітуючи удари серця: "тук-тук, тук-тук" (потрібно буде потестувати передачу вібрації через метал - може не відчуватися).
-При натисканні кнопки, починає грати музика bee gees - stayin' alive (один-два куплети - близько 30-60 сек). Серце "б'ється" і діоди мигають в такт музиці.
-Після проходження циклу, серце знову переходить в пасивний режим.

Живлення 220В - провід до коробочки, або канал для кабелю крізь стіну з можливістю прокинути провід (бажано зарання уточнити, як буде) і розетка за стіною для підключення блоку живлення. 
Варіант з акумуляторами - занадто ненадійний, вбудовувати щось типу сонячних батарей - складно і візуальну картину може зіпсувати
*/

#include <DFPlayer_Mini_Mp3.h> //lib for dfplayer
//#include <SoftwareSerial.h>

//SoftwareSerial Serial_DF(11, 8); // (Rx_pin, Tx_pin) //using a softwareSerial instead of serial, because of debugging through console and uploading a sketch

#define vibro1 5 //pwm
#define vibro2 6 //pwm
#define LED 9 //pwm
#define button_led 3 //pwm
#define button 4
#define busy_pin_DF 2
bool is_button_pressed = false;

unsigned long time_pas = 0;
unsigned long time_pas_prev = 0;    
unsigned long time_pas_cyc = 0;
unsigned long time_pas_cyc_prev = 0;

void setup()
{
	Serial.begin(9600); //initiating serial
	//Serial_DF.begin(9600); //initiating software serial
	//mp3_set_serial(Serial);  //set Serial for DFPlayer-mini mp3 module 
	//mp3_set_volume (25);

	pinMode(vibro1, OUTPUT);
	pinMode(vibro2, OUTPUT);
	pinMode(LED, OUTPUT);
	pinMode(button_led, OUTPUT);
	pinMode(button, INPUT_PULLUP);
	pinMode(busy_pin_DF, INPUT_PULLUP);
	
	digitalWrite(button_led, HIGH);
}

void loop2()
{
	if(digitalRead(button) == LOW){delay(50);is_button_pressed=true;Serial.println("butt");} //read pressed button for sure	

	digitalWrite(vibro1, HIGH);
	for(int i = 0; i < 10; i++)
	{
		analogWrite(LED, i*15);
		delay(30);
	}
	digitalWrite(vibro1, LOW);

	if(digitalRead(button) == LOW){delay(50);is_button_pressed=true;Serial.println("butt");} //read pressed button for sure

	digitalWrite(vibro2, HIGH);
	for(int i = 0; i < 10; i++)
	{
		analogWrite(LED, 150+(i*10));
		delay(20);
	}
	digitalWrite(vibro2, LOW);
	analogWrite(LED, 0);
	delay(500);
}

void loop()
{
	
	vibro();
	if(digitalRead(button) == LOW){delay(50);is_button_pressed=true;Serial.println("butt");}
	
	if(is_button_pressed==true)
	{	
		delay(50);
		digitalWrite(button_led, LOW);
		//mp3_play(1); ////music tempo is 103 beats/min => 103/60 = 0.582
		delay(150); //time to adjust rythm to music - while beats start

		active_mode_test();
		digitalWrite(button_led, HIGH);
		is_button_pressed=false;
	}
}

void passive_mode() //60 beats/min = 1beat/1sec = 300msec 1st half + 200 msec 2nd half + 500msec delay
{	
	Serial.println("passive_mode");
	digitalWrite(vibro1, HIGH);
	for(int i = 0; i < 10; i++)
	{
		analogWrite(LED, i*15);
		delay(30);
	}
	digitalWrite(vibro1, LOW);

	if(digitalRead(button) == LOW){delay(50);is_button_pressed=true;Serial.println("butt");} //read pressed button for sure

	digitalWrite(vibro2, HIGH);
	for(int i = 0; i < 10; i++)
	{
		analogWrite(LED, 150+(i*10));
		delay(20);
	}
	digitalWrite(vibro2, LOW);
	analogWrite(LED, 0);
	delay(500);
}

void active_mode()
{
	while(digitalRead(busy_pin_DF) == LOW) //full cycle = 582 msec
		{
			digitalWrite(vibro1, HIGH);
			digitalWrite(vibro2, HIGH);
			analogWrite(LED, 255);
			delay(200); //200 of 582
			digitalWrite(vibro1, LOW);
			digitalWrite(vibro2, LOW);
			analogWrite(LED, 0);
			delay(382); //+382 of 582
		}
}

void active_mode_test()
{
	for (int i = 0; i < 50; i++)
	{	
		Serial.println("active_mode");
		digitalWrite(vibro1, HIGH);
		digitalWrite(vibro2, HIGH);
		for(int j = 0;j < 10; j++)
		{
			analogWrite(LED, 150+(j*10));
			delay(20);
			Serial.println(j);
		}
		digitalWrite(vibro1, LOW);
		digitalWrite(vibro2, LOW);
		analogWrite(LED, 0);
		delay(382); //+382 of 582
	}
}

void passive_mode_nodelay()  //60 beats/min = 1beat/1sec = 300msec 1st half + 200 msec 2nd half + 500msec delay
{
	time_pas = millis();
  if (time_pas - time_pas_prev <= 500) 
  {
    digitalWrite(vibro1, HIGH);
	for(int i = 0; i <= 10; i++)
	{
		analogWrite(LED, i*15);
		delay(30);
	}
	digitalWrite(vibro1, LOW);

	digitalWrite(vibro2, HIGH);

	for(int i = 0; i <= 10; i++)
	{
		analogWrite(LED, 150+(i*10));
		delay(20);
	}
	digitalWrite(vibro2, LOW);
	analogWrite(LED, 0);
	time_pas_prev = time_pas;
  }
}

void vibro()
{
	digitalWrite(vibro2, HIGH);
	for(int i = 0; i <= 10; i++)
	{
		analogWrite(LED, i*1);
		delay(8);
	}
	digitalWrite(vibro2, LOW);
	delay(150);
	if(digitalRead(button) == LOW){delay(50);is_button_pressed=true;Serial.println("butt");} //read pressed button for sure

	digitalWrite(vibro2, HIGH);
	for(int i = 0; i <= 10; i++)
	{
		analogWrite(LED, 150+(i*10));
		delay(12);
	}
	digitalWrite(vibro2, LOW);
	delay(300);
	if(digitalRead(button) == LOW){delay(50);is_button_pressed=true;Serial.println("butt");} //read pressed button for sure


	analogWrite(vibro2, 200);
	for(int i = 10; i >= 0; i--)
	{
		analogWrite(LED, 150+(i*10));
		delay(12);
	}
	digitalWrite(vibro2, LOW);
	delay(100);
	if(digitalRead(button) == LOW){delay(50);is_button_pressed=true;Serial.println("butt");} //read pressed button for sure

	analogWrite(vibro2, 150);
	for(int i = 10; i >= 0; i--)
	{
		analogWrite(LED, i*10);
		delay(8);
	}
	digitalWrite(vibro2, LOW);
	delay(500);
	if(digitalRead(button) == LOW){delay(50);is_button_pressed=true;Serial.println("butt");} //read pressed button for sure
}