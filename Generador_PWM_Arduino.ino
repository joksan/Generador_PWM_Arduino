/*=============================================================
  Control Directo de PWM para Arduinos basados en AVR de 8 bits
  =============================================================

  Este sketch permite generar una señal PWM con frecuencia y ciclo de
  trabajo arbitrarios mediante el pin 9 (Uno, lilypad, etc.) o bien el
  pin 11 (Mega y Mega ADK).
  Los parametros son establecidos mediante comandos enviados por el
  puerto serie. La misma terminal de Arduino IDE puede ser usada
  para este proposito.

  Parametros de terminal de arduino:
  - Velocidad: 9600 baudios
  - Retorno de linea: CR ó CR/LF

  Parametros adicionales (automatico si se usa la terminal de arduino)
  - Bits de datos: 8
  - Paridad: ninguna
  - Bits de parada: 1

  Comandos:
  f (enter) : permite cambiar la frecuencia (Hz)
  d (enter) : permite cambiar el ciclo de trabajo (%)
  numero (enter) : establece el nuevo valor de frecuencia o ciclo
                   de trabajo
*/

//Cadena de texto que guarda el comando a medida llega por el puerto
String comando;
//Modo de comando. Indica con 'f' que se establece frecuencia y con
//'d' que se establece ciclo de trabajo
char modoCmd = 0;
//Valores de los registros de periodo y ciclo de trabajo
word reg_periodo = 0;
word reg_ciclo = 0;
//Valores de frecuencia (Hz) y ciclo de trabajo (%)
long freq = 1000;
byte ciclo = 50;

void setup() {
  Serial.begin(9600);

  //Se habilita el pin de I/O asociado al canal de PWM utilizado,
  //que sera el canal A del timer 1 (OC1A), segun la tarjeta de
  //arduino empleada
  #if defined(__AVR_ATmega328P__)
    //En las tarjetas basadas en ATMega328 (Uno, lilypad,
    //duemilanove, etc.), la señal PWM se emite por el pin 9
    pinMode(9, OUTPUT);
    digitalWrite(9, LOW);
  #elif defined(__AVR_ATmega2560__)
    //En las tarjetas basadas en ATMega2560 se emplea el pin 11
    pinMode(11, OUTPUT);
    digitalWrite(11, LOW);
  #else
    #error "Tarjeta no soportada"
  #endif
}

void loop() {
  char caracter;

  //La logica se efectua siempre que ingresa un caracter
  if (Serial.available()) {
    caracter = Serial.read();

    if (caracter == 0x0D || caracter == 0x0A) {
      //Si el caracter es de fin de linea, intenta procesar el comando
      if (comando == "f") {
        //El comando "f" indica que los datos ingresados son de
        //frecuencia (en hertz)
        modoCmd = 'f';
        Serial.println("[OK]");
      }
      else if (comando == "d") {
        //El comando "d" indica que los datos ingresados son de ciclo
        //de trabajo (porcentaje)
        modoCmd = 'd';
        Serial.println("[OK]");
      }
      //El resto de comandos se intentan interpretar como numeros
      else if (comando == "0" || comando.toInt() != 0) {
        long valor = comando.toInt();

        //Si se interpreto como numero exitosamente, se actualiza
        //uno de los valores segun el modo de comando
        switch (modoCmd) {
        case 'f':
          //En modo de frecuencia, se verifica que la misma sea
          //lograble con el contador
          if (valor < 245 && valor < 1000000) {
            //Si no es lograble, se emite un mensaje de error
            Serial.println("[ERROR]");
            break;
          }
          //Si es realizable, se actualiza la frecuencia
          freq = valor;
          actualizarPWM();
          break;
        case 'd':
          //En modo de ciclo de trabajo, se verifica que el mismo no
          //exceda 100%
          if (valor > 100) {
            //Si excede 100% se emite un mensaje de error
            Serial.println("[ERROR]");
            break;
          }
          //Si es valido, actualiza el ciclo de trabajo
          ciclo = valor;
          actualizarPWM();
          break;
        default:
          //Si no se ha establecido un modo de comando, se emite un
         //mensaje de error
          Serial.println("[ERROR]");
          break;
        }
      }
      else {
        //Si el comando no se pudo interpretar como numero, reporta un
        //mensaje de error
        if (comando.length() != 0)
          Serial.println("[ERROR]");
      }
      //Tras interpretar el comando, limpia la cadena
      comando = "";
    }
    else if (comando.length() < 16)
      //Todos los demas caracteres se almacenan hasta un maximo de 16
      comando += caracter;
  }
}

void actualizarPWM() {
  //Activa el modulo en modalidad PWM rapido, utilizando ICR1L como
  //registro de cuenta tope (periodo)
  TCCR1A = 0x82;
  TCCR1B = 0x19;

  //Calcula los valores de los registros de acuerdo a las variables
  //de estado
  reg_periodo = F_CPU / freq;
  reg_ciclo = long(ciclo) * reg_periodo / 100;

  //Escribe los registros correspondientes con la nueva configuracion
  OCR1AH = reg_ciclo >> 8;
  OCR1AL = reg_ciclo & 0xFF;
  ICR1H = reg_periodo >> 8;
  ICR1L = reg_periodo & 0xFF;

  //Imprime los valores verdaderos implementados (en caso de errores de
  //aproximacion, se hacen evidentes)
  Serial.print("F = ");
  Serial.println(float(F_CPU) / reg_periodo);
  Serial.print("DC = ");
  Serial.println(float(reg_ciclo) / reg_periodo * 100);
}
