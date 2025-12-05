#include <16F877A.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)

// --- LCD ---
#define LCD_ENABLE_PIN PIN_B2
#define LCD_RS_PIN     PIN_B4
#define LCD_RW_PIN     PIN_B3
#define LCD_DATA4      PIN_D7
#define LCD_DATA5      PIN_D6
#define LCD_DATA6      PIN_D5
#define LCD_DATA7      PIN_D4
#include <lcd.c>

#define IR      PIN_A0 
#define UP      PIN_A1
#define DOWN    PIN_A2
#define RELAY   PIN_C3
#define BUZZER  PIN_C0
#define FLOW    PIN_B0    
#define LED_RED PIN_D1    
#define LED_GR  PIN_D0    

#define IR_ACTIVE_LEVEL 0   // 

volatile unsigned long pulse_count = 0;
unsigned int16 set_ml = 100;
unsigned int16 current_ml = 0;
int1 coLy = 0;
int1 done = 0; 
// --- YF-S402 ---
#INT_EXT
void ext_isr() {
   pulse_count++;
}


float pulses_to_ml(unsigned int16 pulses) {
   return (float)pulses / 4.5;
}

void main() {
   set_tris_a(0b00000111); 
   set_tris_b(0b00000001); 
   set_tris_c(0b11110000); 
   set_tris_d(0b00000000); 
   set_tris_e(0b00000000); // RE not used -> output low
   
   setup_adc_ports(NO_ANALOGS);
   setup_adc(ADC_OFF);

   lcd_init();
   delay_ms(50);
    lcd_gotoxy(1,1);
    printf(lcd_putc,"     LINH THY   ");
    lcd_gotoxy(1,2);
    printf(lcd_putc,"     42200358   ");
    delay_ms(1500);
    lcd_putc('\f'); 
  
   enable_interrupts(INT_EXT);
   ext_int_edge(H_TO_L);   
   enable_interrupts(GLOBAL);

   output_low(RELAY);
   output_low(BUZZER);

   while(TRUE) {
     
      if(!input(UP) && set_ml < 9990) { delay_ms(200); set_ml += 10; }
      if(!input(DOWN) && set_ml > 10) { delay_ms(200); set_ml -= 10; }

      coLy = !(input(IR));

    
      if(coLy) {
       if(!done) {
         output_high(LED_RED);
         output_low(LED_GR);
         output_high(BUZZER);
         delay_ms(1000);
         output_low(BUZZER);

         pulse_count = 0;
         current_ml = 0;
         output_high(RELAY);

         while(current_ml < set_ml && coLy) {
            coLy = !(input(IR));
               if(!coLy) break;
            current_ml = (unsigned int16)pulses_to_ml(pulse_count);
           
            lcd_gotoxy(1,1);
            printf(lcd_putc,"SET:%4lu ml       ", set_ml);
            lcd_gotoxy(1,2);
            printf(lcd_putc,"CO LY :%4lu ml", current_ml);
         }

         output_low(RELAY); 
         output_high(BUZZER);
         delay_ms(1000);
         output_low(BUZZER);
         done = 1;  

      }
      else {
            
            lcd_gotoxy(1,1);
            printf(lcd_putc,"SET:%4lu ml     ", set_ml);
            lcd_gotoxy(1,2);
            printf(lcd_putc,"DAY LY:%4lu", current_ml);

            output_low(LED_RED);
            output_high(LED_GR);

         }
      }
      else {
         
         done = 0;
         lcd_gotoxy(1,1);
         printf(lcd_putc,"SET:%4lu ml     ", set_ml);
         lcd_gotoxy(1,2);
         printf(lcd_putc,"KHONG CO LY     ");
         
         output_low(RELAY);
         output_low(LED_RED);
         output_high(LED_GR);
      }
   }
}
