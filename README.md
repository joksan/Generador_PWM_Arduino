Generador_PWM_Arduino
=====================

Generador de señales PWM de frecuencia y ciclo de trabajo arbitrarios en Arduino.

Este sketch permite usar una tarjeta de arduino para crear señales de PWM con cualquier frecuencia o ciclo de trabajo, limitado solamente por las capacidades del timer de 16 bits del arduino.

La frecuencia minima es de 245Hz, mientras que la maxima depende del reloj del arduino (para 16MHz, el limite es de 8MHz). Notese que la resolucion del PWM disminuye a medida que se eleva la frecuencia.

La configuracion se hace mediante terminal serie. Ver codigo fuente para detalles de como usarlo.

Distribuido bajo licencia GPL v3.