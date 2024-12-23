/*
 * Atmega128 Project
 *
 * Created: 2024-05-29 오후 4:23:23
 * Author : Lee Seung Cheol
 */ 

#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void init();
void txd(char ch);
void txd_string(char *str);
void timerFunc();
void timerGame();
void melodyFunc();
void adcFunc();
void motorFunc();
unsigned char rxd(void);
void motorGame();
void motorGameFunc(char user);

volatile unsigned char Seg[16] = {0x3f, 0x06, 0x5b, 0x4f, 0x66,0x6d,0x7d,0x07,
0x7f, 0x67, 0x77, 0x7c, 0x58,0x5e,0x79, 0x71};
int Count = 0;
volatile unsigned char Fnd[4] = {0};
volatile unsigned char Cmd_U0;
volatile char Hidden_flag=0;
volatile char Int_status=0, Cmd_status=0, PORTA_status=0;


int main(void)
{
     init();
	 
	 
	 txd_string("\n\n\n");
	 txd_string("\n\r***********************************************");
	 txd_string("\n\r******    MicroComputer Term Project     ******");
	 txd_string("\n\r***********************************************");
	 txd_string("\n\r");
	 txd_string("\n\r   ------------- Timer Menu -------------------");
	 txd_string("\n\r   '1' : up counting        '2' : down counting");
	 txd_string("\n\r   '3' : stop counting      '4' : reset counting");
	 txd_string("\n\r   'switch 1' : ★ Hidden Game Stage ★!!");
	 txd_string("\n\r");
	 txd_string("\n\r");
	 txd_string("\n\r   ------------- Motor Menu -------------------");
	 txd_string("\n\r   'z' : 정지               'x' : 저속          ");
	 txd_string("\n\r   'c' : 중속               'v' : 고속          ");
	 txd_string("\n\r   'b' : 정방향             'n' : 역방향         ");
	 txd_string("\n\r   'switch 1: ★ Hidden Game Stage ★!!");
	 txd_string("\n\r");
	 txd_string("\n\r");
	 txd_string("\n\r   ------------- ADC Menu -------------------");
	 txd_string("\n\r   'a' : ADC값(10진수)      's' : ADC값(16진수) ");
	 txd_string("\n\r   'd' : 전압               'f' : 저항          ");
	 txd_string("\n\r");
	 txd_string("\n\r");
	 txd_string("\n\r   ------------- Melody Menu -------------------");
	 txd_string("\n\r   'q' : 시작               'w' : 정지          ");
	 txd_string("\n\r");

 
	
    while (1) 
    {
		switch(Cmd_U0){
			case '1':
			case '2':
			case '3':
			case '4':
				timerFunc();
				break;
			case 'q':
			case 'w':
				melodyFunc();
				break;
			case 'a':
			case 's':
			case 'd':
			case 'f':
				adcFunc();
				break;
			case 'z':
			case 'x':
			case 'c':
			case 'v':
			case 'b':
			case 'n':
				motorFunc();
				break;
			default :
				break;
		
		}
		
		
    }
}

/* 0.1초 간격 TIMER */
void timerFunc()
{
	
	while(1){	
		Fnd[0] = Seg[Count/1000];
		Fnd[1] = Seg[Count/100%10];
		Fnd[2] = Seg[Count/10%10] | 0x80;
		Fnd[3] = Seg[Count%10];
		
		/* 스위치가 눌렸으면 게임모드로 전환 */
		if( Int_status == 1 ){
			Int_status = 0;
			timerGame();
		}
		
		/* 입력 받은게 있으면 main 함수로 */
		if( Cmd_status == 1 ){
			Cmd_status = 0;
			break;
		}
	}
	
	Cmd_status = 0;
	Int_status = 0;
	PORTA = 0x00;
}

void timerGame()
{
	//int value;
	//char msg[50];
	unsigned char char_answer;
	int answer;
	unsigned char user_char;
	
	// 현재 시간을 시드값으로 설정
	srand((unsigned int)time(NULL));
	answer = (rand()%10); // 0~9 랜덤 숫자 생성
	Fnd[0] = 0;
	Fnd[1] = 0;
	Fnd[2] = 0;
	Fnd[3] = Seg[answer];
	
	char_answer = '0'+answer;
	
	txd_string("\n\n\n");
	txd_string("\n\r***********************************************");
	txd_string("\n\r******             Hidden Stage          ******");
	txd_string("\n\r******                                   ******");
	txd_string("\n\r******           ★UP/DOWN game★        ******");
	txd_string("\n\r***********************************************");
	txd_string("\n\r");
	txd_string("\r0~9사이의 숫자를 입력해주세요\n\r");
	while(1){	
		user_char = rxd(); // 사용자로부터 입력을 받을때까지 기다린다. (폴링)
		if( user_char < char_answer )
			txd_string("\rU P\n\r");
		else if( user_char > char_answer )
			txd_string("\rD O W N\n\r");
		else if( user_char == char_answer){
			txd_string("\r정답입니다!!\n\r");
			// FND: Good
			txd_string("\r3초 뒤에 히든 스테이지에서 나갑니다.\n\r");
			txd_string("\r3\n\r");
			_delay_ms(1000);
			txd_string("\r2\n\r");
			_delay_ms(1000);
			txd_string("\r1\n\r");
			_delay_ms(1000);
			break;
		}
		if(Int_status == 0x02){
			txd_string("\r게임을 강제 종료 시킵니다.\n\r");
			_delay_ms(1000);
			break;
		}
	}
	
	Cmd_status = 0;
	Int_status = 0;
	PORTA = 0x00;
	txd_string("\rGood Bye!!\n\r");
}

void melodyFunc()
{
	
	unsigned int tone[8] = {0x3BA, 0x352, 0x2F5, 0x2CA, 0x279, 0x237, 0x1F9, 0x1DC}; // 도:0 레:1 미:2 파:3 솔:4 라:5 시:6 도:7
	unsigned int plane[] = {2,1,0,1,2,2,2,1,1,1,2,4,4,2,1,0,1,2,2,2,1,1,2,1,0,2,1,0,1,2,2,2,1,1,1,2,4,4,2,1,0,1,2,2,2,1,1,2,1,0,-1};
	unsigned int melodyLed[] = {0x10, 0x20, 0x40, 0x80, 0x01, 0x02};
	unsigned int length[] = {3,1,2,2,2,2,4,2,2,4,2,2,4,3,1,2,2,2,2,4,2,2,2,2,6,3,1,2,2,2,2,4,2,2,4,2,2,4,3,1,2,2,2,2,4,2,2,2,2,6};
	int i = 0;
	
	/* PLAY 출력 */
	Fnd[0] = 0x73;
	Fnd[1] = 0x38;
	Fnd[2] = 0x77;
	Fnd[3] = 0x6E;
	
	TIMSK |= (1 << 4) ;
	_delay_ms(1000);
	
	i = 0;
	Cmd_status = 0;
	do {
		OCR1A = tone[plane[i]];
		PORTA = melodyLed[plane[i]];

			
		if(i >= 2 && plane[i-1] == plane[i]){
			TIMSK &= ~(1 << 4);
			_delay_ms(50);
			TIMSK |= (1 << 4);
		}
		switch(length[i]){
			case 1:
				_delay_ms(50);
				break;
			case 2:
				_delay_ms(100);
				break;
			case 3:
				_delay_ms(150);
				break;
			case 4:
				_delay_ms(200);
				break;
			case 5:
				_delay_ms(300);
				break;
			case 6:
				_delay_ms(400);
				break;
				
		}
		_delay_ms(500);
			
		/* 사용자 입력을 받았다면 다시 main 함수로 */
		if( Cmd_status == 1 ){
			if( Cmd_U0 != 'w')
				break;
			TIMSK &= ~(1 << 4); // 일시정지
			while(!(Cmd_U0 == 'q')){}
			TIMSK |= (1 << 4);
		}
	
		i++;
	} while(plane[i] != -1);
	
	Cmd_status = 0;
	Int_status = 0;
	TIMSK &= ~(1 << 4);
	PORTA = 0x00;
}

void adcFunc()
{
	int adc_read, adc,adc_led;
	float vol, res;
	char msg[50];
	unsigned int led[] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF};
	
	while(1){
		 
		ADCSRA |= 0x40; // 시작 ( 기존 값 0x87을(건드리지 않고) 유지한 상태로 해당 비트만 바꾸고 싶음 )
		while((ADCSRA & 0x10) == 0); // A->D 끝났니? (보통 빨리 끝나서 인터럽트 말고 폴링으로 처리 )
		adc_read = (int)ADCL + ((int)ADCH << 8);
		
		adc_led = (int)(adc_read/1023.*8); // 변환된 값을 LED랑 연계
		PORTA = led[adc_led];
		 
		vol = (float)adc_read/1023.*5; // ex) 3.235 volt
		res = 5./vol*200.-200.; // ex) 
	
		 
		/* ADC(1024) -> 16진수로 출력 */
		if(Cmd_U0 == 's'){ 
			Fnd[0] = 0x74; 
			Fnd[1] = Seg[adc_read/0x100];
			Fnd[2] = Seg[(adc_read/0x10)%0x10]; 
			Fnd[3] = Seg[adc_read%0x10];
		}
		 
		/* ADC(1024) -> 10진수로 출력 */
		if(Cmd_U0 == 'a'){ // ADC(1024) -> 10진수 로 출력
			Fnd[0] = Seg[(adc_read)/1000];  
			Fnd[1] = Seg[((adc_read)/100)%10]; 
			Fnd[2] = Seg[(adc_read/10)%10]; 
			Fnd[3] = Seg[adc_read%10];
		}
		 
		/* ADC(1024) -> 전압으로 출력 */
		if(Cmd_U0 == 'd'){
			adc = (int)(1000.*vol); // ex) 3.235 volt -> 3235
			Fnd[0] = Seg[(adc)/1000] | 0x80; // ex) '3' 235 
			Fnd[1] = Seg[((adc)/100)%10]; // ex) 3 '2' 35
			Fnd[2] = Seg[(adc/10)%10]; // ex) 32 '3' 5
			Fnd[3] = 0x3e; // 'v' 문자
		}
		
		/* ADC(1024) -> 저항으로 출력 */ 
		if(Cmd_U0 == 'f'){
			adc = (int)res;
			Fnd[0] = Seg[(adc)/1000] | 0x80; 
			Fnd[1] = Seg[((adc)/100)%10]; 
			Fnd[2] = Seg[(adc/10)%10];
			Fnd[3] = Seg[adc%10];
		}
		 
		sprintf(msg,"\rvol : %.2f [V]  res : %.2f [kOhm]\r", vol, res);
		txd_string(msg);
		 
		_delay_ms(100);
	
		if(Cmd_status == 1){
			Cmd_status = 0;
			break;	
		}
	}
	
	Cmd_status = 0;
	Int_status = 0;
	PORTA = 0x00;
}

void motorFunc()
{	
	OCR2 = 0x00;	
	while(1){
		
		/* PLAY 출력 */
		Fnd[0] = 0x73;
		Fnd[1] = 0x38;
		Fnd[2] = 0x77;
		Fnd[3] = 0x6E;
			
		switch(Cmd_U0){
			case 'z':
				OCR2 = 0x00; // 모터 정지
				break;
			case 'x':
				OCR2 = 0x3F; // 모터 저속
				break;
			case 'c':
				OCR2 = 0x7F; // 모터 중속
				break;
			case 'v':
				OCR2 = 0xFF; // 모터 고속
				break;
			case 'b': // 정방향
				OCR2 = 0x00;
				TCCR2 = 0x6B;
				PORTB = 0x00;
				break;
			case 'n': // 역방향
				OCR2 = 0x00; // 일단 멈추고 방향 전환
				TCCR2 = 0x7B;
				PORTB = 0x40;
				break;
		}
		
		if( Int_status == 1 ){ // 스위치1을 눌렀다면 게임모드로 전환
			Int_status = 0;
			OCR2 = 0x00;
			motorGame();
		}
		
		if( Cmd_status == 1 ){ // 사용자가 무언가를 입력 했다면 다시 main 함수로
			Cmd_status = 0;
			break;
		}
		
	}
	
	Cmd_status = 0;
	Int_status = 0;
	PORTA = 0x00;
	OCR2 = 0x00;
}

void motorGame()
{
	char user;
	
	txd_string("\r\n\r");
	txd_string("\r********************************************\n\r");
	txd_string("\r       운전면허시험장에 오신 걸 환영합니다. \n\r");
	txd_string("\r********************************************\n\r");
	txd_string("\r\n\r");
	_delay_ms(500);
	txd_string("\r\n\r");
	txd_string("\r********************************************\n\r");
	txd_string("\r       지금부터 도로주행시험을 시작합니다. \n\r");
	txd_string("\r********************************************\n\r");
	txd_string("\r\n\r");
	_delay_ms(500);
	txd_string("\r*************************** 도움말 ************************\n\r");
	txd_string("\r                                                           \n\r");
	txd_string("\r                'd':전진 기어, 'r':후진 기어                \n\r");
	txd_string("\r                                                           \n\r");
	txd_string("\r   'z':좌측 방향 지시등, 'x': 비상등, 'c: 우측 방향 지시등    \n\r");
	txd_string("\r                                                           \n\r");
	txd_string("\r's': 브레이크 '1':저속 페달, '2':중속 페달, '3':고속 페달\n\r");
	txd_string("\r                                                           \n\r");
	txd_string("\r***********************************************************\n\r");
	txd_string("\r\n\r");
	
	PORTA = 0x00;
	txd_string("\r********************************************\n\r");
	txd_string("\r       (1/10). 기어를 'D' 로 바꿔주세요. \n\r");
	txd_string("\r********************************************\n\r");
	txd_string("\r\n\r");
	while(1){
		user = rxd();
		motorGameFunc(user);
		if(user == 'd')
			break;
	}
	
	txd_string("\r******************************\n\r");
	txd_string("\r       (2/10). 출발하세요.   \n\r");
	txd_string("\r******************************\n\r");
	txd_string("\r\n\r");
	while(1){
		user = rxd();
		motorGameFunc(user);
		if(user == '1')
			break;
	}
	
	txd_string("\r*************************************\n\r");
	txd_string("\r       (3/10). 중속으로 밟으세요.   \n\r");
	txd_string("\r*************************************\n\r");
	txd_string("\r\n\r");
	while(1){
		user = rxd();
		motorGameFunc(user);
		if(user == '2')
			break;
	}
	
	txd_string("\r********************************************************\n\r");
	txd_string("\r      (4/10). 앞에 보이는 4거리에서 좌회전 해야합니다. \n\r");
	txd_string("\r             좌측 방향 지시등부터 키세요.              \n\r");
	txd_string("\r********************************************************\n\r");
	txd_string("\r\n\r");
	while(1){
		user = rxd();
		motorGameFunc(user);
		if(user == 'z')
			break;
	}
	
	txd_string("\r*******************************************\n\r");
	txd_string("\r***** (5/10). 좌측 방향 지시등을 끄세요. *****\n\r");
	txd_string("\r*******************************************\n\r");
	txd_string("\r\n\r");
	while(1){
		user = rxd();
		motorGameFunc(user);
		if(user == 'x')
			break;
	}
	
	txd_string("\r **********************************************************\n\r");
	txd_string("\r      (6/10). 어린이보호지역에 진입하셨습니다. 서행하세요.  \n\r");
	txd_string("\r **********************************************************\n\r");
	txd_string("\r\n\r");
	while(1){
		user = rxd();
		motorGameFunc(user);
		if(user == '1')
			break;
		txd_string("\r!!! 어린이 보호구역에서는 속도를 항상 줄이세요. !!!\n\r");
	}
	
	txd_string("\r**********************************************\n\r");
	txd_string("\r***** (7/10). 도로주행은 이걸로 끝났습니다. ***** \n\r");
	txd_string("\r   이제 주차를 하기 위해서 후진 기어를 넣으세요. \n\r");
	txd_string("\r**********************************************\n\r");
	txd_string("\r\n\r");
	while(1){
		user = rxd();
		motorGameFunc(user);
		if(user == 'r')
			break;
	}
	
	txd_string("\r*******************************\n\r");
	txd_string("\r     (8/10). 후진 하세요. \n\r");
	txd_string("\r*******************************\n\r");
	txd_string("\r\n\r");
	while(1){
		user = rxd();
		motorGameFunc(user);
		if(user == '1')
			break;
	}
	
	txd_string("\r*******************************\n\r");
	txd_string("\r***** (9/10). 정지 하세요. *****\n\r");
	txd_string("\r*******************************\n\r");
	txd_string("\r\n\r");
	while(1){
		user = rxd();
		motorGameFunc(user);
		if(user == 's')
		break;
	}
	
	txd_string("\r************************************************************\n\r");
	txd_string("\r*****(10/10). 수고하셨습니다. 비상등을 키고 하차해주세요. *****\n\r");
	txd_string("\r************************************************************\n\r");
	txd_string("\r\n\r");
	while(1){
		user = rxd();
		motorGameFunc(user);
		if(user == 'p')
			break;
	}
	
	txd_string("\r***********************************************************\n\r");
	txd_string("\r*****!!! 축하합니다. 운전면허시험에 통과하셨습니다. !!! ******\n\r");
	txd_string("\r***********************************************************\n\r");
	_delay_ms(3000);
	txd_string("\r운전면허시험을 종료합니다.\n\r");
	txd_string("\r\n\r");
	_delay_ms(5000);
	PORTA = 0;
	PORTA_status = 0;
	Cmd_status = 0;
	OCR2 = 0x00;
	Int_status = 0;
}

void motorGameFunc(char user)
{
	 switch(user){
		 case 's':
			OCR2 = 0x00; // 모터 정지
			txd_string("\r<브레이크>\n\r");
			txd_string("\r\n\r");
			break;
		 case '1':
			OCR2 = 0x3F; // 모터 저속
			txd_string("\r<저속>\n\r");
			txd_string("\r\n\r");
			break;
		 case '2':
			OCR2 = 0x7F; // 모터 중속
			txd_string("\r<중속>\n\r");
			txd_string("\r\n\r");
			break;
		 case '3':
			OCR2 = 0xFF; // 모터 고속
			txd_string("\r<고속>\n\r");
			txd_string("\r\n\r");
			break;
		 case 'd':
			OCR2 = 0x00;
			TCCR2 = 0x6B;
			PORTB = 0x00;
			txd_string("\r<기어 변환: 'D'>\n\r");
			txd_string("\r\n\r");
			break;
		 case 'r': // 역방향
			OCR2 = 0x00; // 일단 멈추고 방향 전환
			TCCR2 = 0x7B;
			PORTB = 0x40;
			txd_string("\r<기어 변환: 'R'>\n\r");
			txd_string("\r\n\r");
			break;
		case 'z': // 좌측 방향 지시등
			PORTA_status = 1;
			txd_string("\r<좌측 방향 지시등>\n\r");
			txd_string("\r\n\r");
			break;
		case 'c': // 우측 방향 지시등
			PORTA_status = 2;
			txd_string("\r<우측 방향 지시등>\n\r");
			txd_string("\r\n\r");
			break;
		case 'p': // 비상등
			PORTA_status = 3;
			txd_string("\r<비상등 ON>\n\r");
			txd_string("\r\n\r");
			break;
		case 'x': // 끄기
			PORTA_status = 4;
			txd_string("\r<OFF>\n\r");
			txd_string("\r\n\r");
			break;
	 }
	 Cmd_status = 0;
}

void txd(char ch)
{
	while(!(UCSR0A & 0x20)); // UDRE0=1 ?(전송데이터 empty?)
	UDR0 = ch;
}

void txd_string(char *str)
{
	int i=0;
	
	while(1) {
		if (str[i] == '\0') break;
		txd(str[i++]);
	}
}

void init()
{

	// I/O Port 초기화
	DDRA = 0xFF;
	DDRB = 0xD0;
	DDRC = 0xFF;
	DDRG = 0x0F;
	DDRE = 0x02;
	DDRF = 0x00;

	// 외부 인터럽트 (Switch1/2) Enable
	EICRB = 0x0A;
	EIMSK = 0x30;

	// Timer/Counter0 초기화 (FND)
	TCCR0 = 0x06;
	TCNT0 = 0x83;
	
	//Timer/Counter1 초기화 (BUZZER)
	TCCR1A = 0x00;
	TCCR1B = 0x0A;
	TCCR1C = 0x00;
	OCR1A = 0x3BA;
	
	// Timer/Counter2 초기화
	/* 모터 초기 설정 */
	TCCR2 = 0x6B;
	PORTB = 0x00;

	// Timer/Counter3 초기화
	TCCR3A = 0x00;
	TCCR3B = 0x0B;
	TCCR3C = 0x00;
	OCR3A = 0x1869;
	
	// Timer Interrupt Enable
	TIMSK = 0x01 | 0x04;
	ETIMSK = 0x10;

	// UART 초기화
	UCSR0A = 0x00;
	UCSR0B = 0x98; //0x18
	UCSR0C = 0x06;
	UBRR0H = 0;
	UBRR0L = 0x67;

	// AD 변환기 초기화
	ADMUX = 0x00;
	ADCSRA = 0x87;

	// Global 인터럽트 enable
	sei();
}

ISR(TIMER0_OVF_vect) // 2ms 간격으로 FND 1개씩 display
{
	static int count = 0;
	TCNT0 = 0x83;
	PORTC = Fnd[count];
	PORTG = 8 >> count;
	count++;
	count %= 4;
}

/* BUZZER */
ISR(TIMER1_COMPA_vect)
{
	static char on_off = 0x00;
	
	if(on_off == 0x00)
		PORTB = 0x10;
	else
		PORTB = 0x00;

	on_off++;
	on_off %= 2;
}

/* Count 증가, 감소 */
/* 0.1s 마다 */
ISR(TIMER3_COMPA_vect)
{
	static int cnt=0, prestatus=0;
	
	/* Timer */
	if( Cmd_U0 == '1' ){
		Count++;
		if( Count == 10000 ) Count = 0;
	}
	if( Cmd_U0 == '2'){
		Count--;
		if( Count == 0 ) Count = 9999;
	}
	if( Cmd_U0 == '4')
		Count = 0;
	
	/* Motor Game */
	if( cnt==20 ){ // 2초 간격으로 깜빡인다
		cnt=0;
		if( PORTA_status == 1 ){
			if( prestatus == 1 )
				PORTA = 0b11110000;
			else
				PORTA = 0b00000000;
		}
		if( PORTA_status == 2 ){
			if( prestatus == 1 )
				PORTA = 0b00001111;
			else
				PORTA = 0b00000000;
		}
		if( PORTA_status == 3 ){
			if( prestatus == 1 )
				PORTA = 0b11111111;
			else
				PORTA = 0b00000000;
		}
		if( PORTA_status == 4 ){
			PORTA = 0x00;
	
		}
		
		prestatus++;
		prestatus %= 2;
	}
	cnt++;
}

ISR(USART0_RX_vect)  // keyboard input 시 interrupt 발생
{
	Cmd_U0 = UDR0; // 데이터가 있으면 Cmd_U0에 할당
	Cmd_status = 0x01;
}

unsigned char rxd(void)
{
	unsigned char ch;
	while(!(UCSR0A & 0x80));
	ch = UDR0;
	return ch;
}

ISR(INT4_vect)
{
	Int_status = 0x01;
}

ISR(INT5_vect)
{
	Int_status = 0x02;
}


