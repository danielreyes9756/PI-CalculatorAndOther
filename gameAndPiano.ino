//Variables

int iCount=0,displayCount=0,s=0,s2=0,m=0,m2=0,timbre=0,pulse=0,p=0,iCount2=0,turno=0,modo=0;
byte vector[]={0x3F, 0x6, 0x5b, 0x4F, 0x66, 0x6D, 0x7D, 0x7, 0x7F, 0x6F};
long ini;
String cadena="",cadena2="";
char aux='\0';
boolean permite=false,startGame=false,win=false;

char tablero[3][3]= { {'z','z','z'},
                      {'z','z','z'},
                      {'z','z','z'}};

void setup() {
  cli();
  // modo normal de funcionamiento
  TCCR1A = 0;
  TCCR1B = 0;
  // cuenta inicial a cero
  TCNT1 = 0;
  // mode CTC
  TCCR1B |= (1 << WGM12);
  // prescaler N = 1024
  TCCR1B |= (1 << CS12) | (1 << CS10);
  // fintr = fclk/(N*(OCR1A+1)) --> OCR1A = [fclk/(N*fintr)] - 1
  // para fintr = 100Hz --> OCR1A = [16*10^6/(1024*100)] -1 = 155,25 --> 155

  OCR1A = 77; // para 200 Hz programar OCR1A = 77
  // enable timer1 compare interrupt (OCR1A)
  TIMSK1 |= (1 << OCIE1A);
  // habilitar interrupciones
  sei();

  //Serial3
  Serial.begin(9600);
  Serial3.begin(9600);
  //Display On
  Serial3.write(0xFE);
  Serial3.write(0x41);
  //Clear Screen
  Serial3.write(0xFE);
  Serial3.write(0x51);
  //Cursor Home
  Serial3.write(0xFE);
  Serial3.write(0x46);
  //Underline Cursor On
  Serial3.write(0xFE);
  Serial3.write(0x47);

  //7Segmentos
  DDRA = B11111111;

  //Botones
  DDRC = B00000001;
  PORTC = B11111000;

  //Lineas de teclado
  DDRL = B00001111;
  PORTL = B11110000;

  ini = millis();
  Serial3.print("Modo Calculadora");
  delay(2000);
  clearAll();
}


void loop() {
    if(modo==1){
      modo_game();
    }else if(modo==2){
      modo_piano();
    }else if(modo==0){
      modo_calculadora();
    }
}

ISR(TIMER1_COMPA_vect) {
  if(modo==1){
    isr_Game();
  }else if(modo==2){
    isr_Piano();
  }else if(modo==0){
    isr_Calculadora();
  }
}

void isr_Calculadora() {
  if (iCount == 200) {
    Clock();
    iCount = 0;
  }
  if (displayCount == 4) {
    displayCount = 0;
  }
  if (displayCount == 0) {
    PORTA = vector[s];
    PORTL = 0b11111110;
    checkKB(0);
  }
  if (displayCount == 1) {
    PORTA = vector[s2];
    PORTL = 0b11111101;
    checkKB(1);
  }
  if (displayCount == 2) {
    PORTA = vector[m];
    PORTL = 0b11111011;
    checkKB(2);
  }
  if (displayCount == 3) {
    PORTA = vector[m2];
    PORTL = 0b11110111;
  }
  displayCount++;
  iCount++;
  if (pulse == 1) {
    if ((millis() - ini) >= 200) {
      ini = millis();
      permite=true;
      tone(37,timbre,100);
      Serial3.write(aux);
      cadena += aux;
    }
    pulse = 0;
  }
}

void isr_Game() {
  if(iCount2 == 200 && startGame){
    Clock();
    iCount2=0;
  }
  if(iCount2%20==0) {
    if(iCount2==200){iCount2=0;} 
    if(p==0){
      p=1;
    } else if(p==1) {
      p=0b11;
    } else if(p==0b11){
      p=0b111;
      } else if(p==0b111){
      p=0b1111;
      } else if(p==0b1111){
      p=0b11111;
      }  else if(p==0b11111){
      p=0b111111;
      } else if(p==0b111111){
        p=0b111110;
      } else if(p==0b111110){
        p=0b111100;
      } else if(p==0b111100){
        p=0b111000;
      } else if(p==0b111000){
        p=0b110000;
      } else if(p==0b110000){
        p=0b100000;
      } else if(p==0b100000){
        p=0;
      }
  }
  if (displayCount == 4) {
    displayCount = 0;
  }
  if (displayCount == 0) {
    if(startGame)PORTA = vector[s];
    else PORTA = p;
    PORTL = 0b11111110;
    checkKB(0);
  }
  if (displayCount == 1) {
    if(startGame)PORTA = vector[s2];
    else PORTA = p;
    PORTL = 0b11111101;
    checkKB(1);
  }
  if (displayCount == 2) {
    if(startGame)PORTA = vector[m];
    else PORTA = p;
    PORTL = 0b11111011;
    checkKB(2);
  }
  if (displayCount == 3) {
    if(startGame)PORTA = vector[m2];
    else PORTA = p;
    PORTL = 0b11110111;
  }
  displayCount++;
  iCount2++;
  if (pulse == 1) {
    if ((millis() - ini) >= 200) {
      ini = millis();
    }
    pulse = 0;
  }
}

//----------------------------------------GAME---------------------------------------//

void modo_game(){
  if(permite){
    if(digitalRead(34)==0){
      clearAll();
      startGame=true;
      permite=false;
    } 
  }
  if(digitalRead(34)==0 && digitalRead(31)==0){
     clearAll();
     startGame=false;
     permite=true;
     turno=0;
     Serial3.print("Modo juego 3 en raya");
     delay(1000);
     s=0;s2=0;m=0;m2=0;
     clearAll();
     for(int i = 0; i<3;i++){
        for(int j=0; j<3;j++){
          tablero[i][j]='z';
        }
     }
  }
  if(startGame){
   pulsacionTurno(); 
   aux='0';
   int x = comprobarGanador();
   
   if(x==1){Serial3.write(0xFE);Serial3.write(0x45);Serial3.write(0x54); Serial3.print("Gano player1"); startGame=false;}
   if(x==2){Serial3.write(0xFE);Serial3.write(0x45);Serial3.write(0x54);Serial3.print("Gano player2"); startGame=false;}
   if(x==9){Serial3.write(0xFE);Serial3.write(0x45);Serial3.write(0x54);Serial3.print("Empate"); startGame=false;}
  }
}

void pulsacionTurno() {
  switch(aux){
      case '1': 
      if(tablero[0][0]=='z' && turno==0){ tablero[0][0]='O'; escribir(aux,turno); delay(200); turno=1;}
      if(tablero[0][0]=='z' && turno==1){ tablero[0][0]='X'; escribir(aux,turno); delay(200); turno=0;}
      break;
      case '2': 
      if(tablero[0][1]=='z' && turno==0){ tablero[0][1]='O'; escribir(aux,turno); delay(200); turno=1;}
      if(tablero[0][1]=='z' && turno==1){ tablero[0][1]='X'; escribir(aux,turno); delay(200); turno=0;}
      break;
      case '3': 
      if(tablero[0][2]=='z' && turno==0){ tablero[0][2]='O'; escribir(aux,turno); delay(200); turno=1;}
      if(tablero[0][2]=='z' && turno==1){ tablero[0][2]='X'; escribir(aux,turno); delay(200); turno=0;}
      break;
      case '4': 
      if(tablero[1][0]=='z' && turno==0){ tablero[1][0]='O'; escribir(aux,turno); delay(200); turno=1;}
      if(tablero[1][0]=='z' && turno==1){ tablero[1][0]='X'; escribir(aux,turno); delay(200); turno=0;}
      break;
      case '5': 
      if(tablero[1][1]=='z' && turno==0){ tablero[1][1]='O'; escribir(aux,turno); delay(200); turno=1;}
      if(tablero[1][1]=='z' && turno==1){ tablero[1][1]='X'; escribir(aux,turno); delay(200); turno=0;}
      break;
      case '6': 
      if(tablero[1][2]=='z' && turno==0){ tablero[1][2]='O'; escribir(aux,turno); delay(200); turno=1;}
      if(tablero[1][2]=='z' && turno==1){ tablero[1][2]='X'; escribir(aux,turno); delay(200); turno=0;}
      break;
      case '7': 
      if(tablero[2][0]=='z' && turno==0){ tablero[2][0]='O'; escribir(aux,turno); delay(200); turno=1;}
      if(tablero[2][0]=='z' && turno==1){ tablero[2][0]='X'; escribir(aux,turno); delay(200); turno=0;}
      break;
      case '8': 
      if(tablero[2][1]=='z' && turno==0){ tablero[2][1]='O'; escribir(aux,turno); delay(200); turno=1;}
      if(tablero[2][1]=='z' && turno==1){ tablero[2][1]='X'; escribir(aux,turno); delay(200); turno=0;}
      break;
      case '9': 
      if(tablero[2][2]=='z' && turno==0){ tablero[2][2]='O'; escribir(aux,turno); delay(200); turno=1;}
      if(tablero[2][2]=='z' && turno==1){ tablero[2][2]='X'; escribir(aux,turno); delay(200); turno=0;}
      break;
    }
}

void escribir(char aux,int turno) {
  int i=0;
  if(aux=='1'||aux=='2'||aux=='3'){
    i=0;
  }
  if(aux=='4'||aux=='5'||aux=='6'){
    i=40;
  }
  if(aux=='7'||aux=='8'||aux=='9'){
    i=20;
  }
  
  if(aux=='1' || aux=='4' || aux=='7'){
    i+=8;
  }
  if(aux=='2' || aux=='5' || aux=='8'){
    i+=9;
  }
  if(aux=='3' || aux=='6' || aux=='9'){
    i+=10;
  }
  
    Serial3.write(0xFE);
    Serial3.write(0x45);
    Serial3.write(i);
    if(turno==0){Serial3.print('O');}
    if(turno==1){Serial3.print('X');}
}

int comprobarGanador() {
  //VICTORIA TODAS LAS FILAS
  if(tablero[0][0]==tablero[0][1] && tablero[0][1]==tablero[0][2]){
    if(tablero[0][0]=='O'){ return 1;}
    if(tablero[0][0]=='X'){ return 2;}
  }
  if(tablero[1][0]==tablero[1][1] && tablero[1][1]==tablero[1][2]){
    if(tablero[1][0]=='O'){ return 1;}
    if(tablero[1][0]=='X'){ return 2;}
  }
  
  if(tablero[2][0]==tablero[2][1] && tablero[2][1]==tablero[2][2]){
    if(tablero[2][2]=='O'){ return 1;}
    if(tablero[2][2]=='X'){ return 2;}
  }
  
  //VICTORIA DIAGONALES
  if(tablero[0][0]==tablero[1][1] && tablero[1][1]==tablero[2][2]){
    if(tablero[1][1]=='O'){ return 1;}
    if(tablero[1][1]=='X'){ return 2;}
  }
  if(tablero[0][2]==tablero[1][1] && tablero[1][1]==tablero[2][0]){
    if(tablero[1][1]=='O'){ return 1;}
    if(tablero[1][1]=='X'){ return 2;}
  }
  //VICTORIA COLUMNAS
  if(tablero[0][0]==tablero[1][0] && tablero[1][0]==tablero[2][0]){
    if(tablero[0][0]=='O'){ return 1;}
    if(tablero[0][0]=='X'){ return 2;}
  }
  
  if(tablero[0][1]==tablero[1][1] && tablero[1][1]==tablero[2][1]){
    if(tablero[0][1]=='O'){ return 1;}
    if(tablero[0][1]=='X'){ return 2;}
  }
  
  if(tablero[0][2]==tablero[1][2] && tablero[1][2]==tablero[2][2]){
    if(tablero[0][2]=='O'){ return 1;}
    if(tablero[0][2]=='X'){ return 2;}
  }
  int complete=0;
  for(int i = 0; i<3;i++){
        for(int j=0; j<3;j++){
          if(tablero[i][j]!='z'){
            complete++;
          }
        }
     }
     if(complete==9){return 9;}
  return 3;
}
//-----------------------------------------RELOJ---------------------------------------//
//Comprobar el reloj
void Clock() {
  s++;
  if (s > 9) {s = 0;s2++;}
  if (s2 > 5) {s2 = 0;m++;}
  if (m > 9) {m = 0;m2++;}
  if (m2 > 9) {m2 = 0;}
}

void checkKB(int a) {
  if (digitalRead(42) == 0 && digitalRead(45)==0) {
    while(digitalRead(42) == 0 && digitalRead(45)==0){}
    if(a==0 && modo==1){
      modo=0;
      s=0;s2=0;m=0;m2=0;
      startGame=false;
      permite=false;
      clearAll();
      Serial3.write("Modo Calculadora");
      clearAll();
      }
  }
  if (digitalRead(44) == 0 && digitalRead(45)==0) {
    while(digitalRead(44) == 0 && digitalRead(45)==0){}
    if(a==0 && modo==0){
      modo=1; 
      permite=true;
      s=0;s2=0;m=0;m2=0;
      clearAll();
      Serial3.write("Modo Juego");
      }
  }
  if (digitalRead(42) == 0) {
    if (cadena.length() < 20) {
      if (a == 0) {aux = '1'; if(modo==2){timbre=3000;}}
      if (a == 1) {aux = '2'; if(modo==2){timbre=3100;}}
      if (a == 2) {aux = '3'; if(modo==2){timbre=3200;}}
      pulse = 1;
    }
  }
  if (digitalRead(43) == 0) {
    if (cadena.length() < 20) {
      if (a == 0) {aux = '4'; if(modo==2){timbre=3300;}}
      if (a == 1) {aux = '5'; if(modo==2){timbre=3400;}}
      if (a == 2) {aux = '6'; if(modo==2){timbre=3500;}}
      pulse = 1;
    }
  }
  if (digitalRead(44) == 0) {
    if (cadena.length() < 20) {
      if (a == 0 && modo==0 || a == 0 && startGame || modo == 2){aux = '7'; if(modo==2){timbre=3600;}}
      if (a == 1) {aux = '8'; if(modo==2){timbre=3700;}}
      if (a == 2) {aux = '9'; if(modo==2){timbre=3800;}}
      pulse = 1;
    }
  }
  if (digitalRead(45) == 0) {
    if(a==0 && aux=='#' && modo!=2){ 
      Serial.println("hola");
      modo=2;
      s=0;s2=0;m=0;m2=0;
      startGame=false;
      permite=true;
      clearAll();
      Serial3.write("Modo Piano");
      clearAll();
    }
    if (a == 0 && modo==0) {aux = '*';}
    if (a == 2) {aux = '#';}
    if (cadena.length() < 20) {
      if (a == 1) {
        aux = '0';
        pulse = 1;
        if(modo==2){
          timbre=3900;
        }
      }
    }
  }
}

void clearAll() {
  Serial3.write(0xFE);
  Serial3.write(0x51);
  if(!modo){
    Serial3.write(0xFE);
    Serial3.write(0x45);
    Serial3.write(0x53);
    Serial3.write('0');
    permite=false;
  }
  Serial3.write(0xFE);
  Serial3.write(0x46);
  aux = '\0';
  cadena = "";
}

void cleanOne() {
  int z = cadena.length()-1;
  if (z >= 0) {
    char a = cadena.charAt(z);
    if(a=='-' || a=='/' || a=='*' || a=='+'){permite=true;}else{permite=false;}
    Serial3.write(0xFE);
    Serial3.write(0x4E);
    String cadena2 = cadena.substring(0,cadena.length()-1);
    cadena = cadena2;
  }
  aux = '\0';
}
long calcRes(String sAux[],char charAux[]){
  int punteroS = 0;
  int punteroChar = 0;
  for (int i = 0; i < cadena.length(); i++) {
        if (cadena.charAt(i) != '+' && cadena.charAt(i) != '-' && cadena.charAt(i) != '*' && cadena.charAt(i) != '/' && cadena.charAt('\0')) {
          sAux[punteroS] += cadena.charAt(i);
        } else {
          charAux[punteroS] = cadena.charAt(i);
          punteroChar++;
          punteroS++;
        }
      }
  long res = sAux[0].toInt();
      if (punteroChar > 0) {
        for (int i = 0; i < punteroChar; i++) {
          if(charAux[i]=='+'){res+=sAux[i+1].toInt();}
          if(charAux[i]=='-'){res-=sAux[i+1].toInt();}
          if(charAux[i]=='*'){res*=sAux[i+1].toInt();}
          if(charAux[i]=='/'){res/=sAux[i+1].toInt();}
        }
    }
    return res;
}

void enterClean(long res){
   String res2 = String(res);
      Serial3.write(0xFE);
      Serial3.write(0x45);
      Serial3.write(0x53);
      for(int i=0; i<20;i++){
        Serial3.write(0xFE);
        Serial3.write(0x4E);
      }
      Serial3.write(0xFE);
      Serial3.write(0x45);
      Serial3.write(0x53-res2.length()+1);
      Serial3.print(res);
      res=0;
      Serial3.write(0xFE);
      Serial3.write(0x46);
      permite=false;
      delay(250);
}

void modo_calculadora() {
  if (aux == '*') {clearAll();}
    if (aux == '#') {
    if ((millis() - ini) >= 300) {
      ini = millis();
      cleanOne();
    }
    aux = '\0';
  }
    //Center = resolver
    if (digitalRead(33) == LOW) {
      String sAux[10];
      char charAux[10];
      long res = calcRes(sAux,charAux);
      while (cadena.length()>0) {
        cleanOne();
      }
      cleanOne();
      //limpia el resultado anterior si hay e imprime el nuevo
      enterClean(res);
    }
    
    if(permite==true){
          //Up = suma
      if (digitalRead(34) == LOW) {
        permite=false;
        Serial3.write("+");
        cadena += "+";
        delay(250);
      }
          //Down = resta
      if (digitalRead(31) == LOW) {
          permite=false;
          Serial3.write("-");
          cadena += "-";
          delay(250);
      }
          //Right = multiplicacion
      if (digitalRead(30) == LOW) {
        permite=false;
        Serial3.write("*");
        cadena += "*";
        delay(250);
      }
          //Left = division
      if (digitalRead(32) == LOW) {
        permite=false;
        Serial3.write("/");
        cadena += "/";
        delay(250);
      }
   }
}

//----------------------------------------------MODO PIANO--------------------------------------------//
void isr_Piano() {
    String x= String(aux);
    s=x.toInt();
  if (displayCount == 4) {
    displayCount = 0;
  }
  if (displayCount == 0) {
    PORTA = vector[s];
    PORTL = 0b11111110;
    checkKB(0);
  }
  if (displayCount == 1) {
    PORTA = 0;
    PORTL = 0b11111101;
    checkKB(1);
  }
  if (displayCount == 2) {
    PORTA = 0;
    PORTL = 0b11111011;
    checkKB(2);
  }
  if (displayCount == 3) {
    PORTA = 0;
    PORTL = 0b11110111;
  }
  displayCount++;
  
  if (pulse == 1) {
    if ((millis() - ini) >= 200) {
      ini = millis();
      permite=true;
      tone(37,timbre,100);
    }
    pulse = 0;
  }
}

void modo_piano(){
  if(digitalRead(34) == 0 && digitalRead(31)==0){
      modo=0;
      s=0;s2=0;m=0;m2=0;
      startGame=false;
      permite=false;
      clearAll();
      Serial3.write("Modo Calculadora");
      clearAll();  
  }
}
  
