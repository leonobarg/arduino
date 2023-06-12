#include <SPI.h>  // INCLUYE LA LIBRERIA SPI
#include <UIPEthernet.h>  //INCLUYE LIBRERIA ETHERNET (PARA PODER TRABAJAR CON LA PLACA ENC28J60)
#include <DHT.h>

#define DHTPIN1 8
#define DHTPIN2 9
#define DHTTIPO DHT11

DHT dht1(DHTPIN1, DHTTIPO);
DHT dht2(DHTPIN2, DHTTIPO);

byte mac[] = { 0xB4, 0x8C, 0x9D, 0x2D, 0x1E, 0xA5 }; 
byte ip[] = {192,168,102,205}; //DEFINO DIRECCION IP PARA EL MODULO ETHERNET
EthernetServer server(80); // CONFIGURO EL PUERTO POR EL CUAL SE VAN A RECIBIR LAS SOLICITUDES DE CONEXIONES ENTRANTES

int estado=7;     // PUESTA EN MARCHA Y PARADA
int operacion=6;  // MODO AUTOMATICO O MANUAL
int ventilacion =5;
int calefaccion =4;
int riego = 3;
byte sensor = A0;   //ENTRADA ANALOGICA DEL SENSOR DE SUELO

int cont1; //CONTADOR DE ESTADO
int cont2; //CONTADOR DE OPERACION
int cont3; //CONTADOR DE VENTILACION
int cont4; //CONTADOR DE CALEFACCION
int cont5; //CONTADOR DE SISTEMA DE RIEGO
void setup()
{
  Ethernet.begin(mac, ip); //INICIALIZA LA BIBLIOTECA ETHERNET Y CONFIGURA LA RED
  server.begin(); //INICIALIZA EL SERVIDOR PARA QUE COMIENCE A ESCUCHAR LAS CONEXIONES ENTRANTES
  dht1.begin(); //INICIALIZO SENSOR 1
  dht2.begin(); //INICIALIZO SENSOR 2

  //DEFINO PIN ANALOGICO COMO ENTRADA
  pinMode(sensor, INPUT);

  //DEFINO LOS PINES COMO SALIDA
  pinMode(estado, OUTPUT);  
  pinMode(operacion, OUTPUT);
  pinMode(ventilacion, OUTPUT);
  pinMode(calefaccion, OUTPUT); 
  pinMode(riego, OUTPUT);

  //ESCRIBO EN LOS PUERTOS UNA SEÑAL BAJA
  digitalWrite(ventilacion, LOW);
  digitalWrite(estado, LOW);
  digitalWrite(calefaccion, LOW);
  digitalWrite(operacion, LOW); 
  digitalWrite(riego, LOW);
}
void loop()
{
  float h1;
  float h2;
  float t1;
  float t2;
  float hs;
  EthernetClient client = server.available();
  
  if (client) 
  {
    bool currentLineIsBlank = true;
    String cadena = "";
    while (client.connected()) //MIENTRAS EL CLIENTE ESTE CONECTADO
    {  
      if (client.available()) //SI SE DEVUELVE EL NRO DE BYTES DISPONIBLES PARA LEER 
      {
        char c = client.read();//LEE EL SIGUIENTE BYTE RECIBIDO DEL SERVIDOR AL QUE ESTA CONECTADO EL CLIENTE 
       
         if (cadena.length()<50) 
        {
          cadena.concat(c); //Agrega el parámetro a una cadena.

          int posicion = cadena.indexOf("data");
          String command = cadena.substring(posicion);
          
          //SI SE PRESIONO ALGUNO DE LOS PULSADORES ELIGE QUE SUCEDERA
          if (command == "data1=0")
          {
            cont1=0;               //Configura ESTADO en DETENIDO
          }
          else if (command == "data1=1")
          {
            cont1=1;              //Configura ESTADO en FUNCIONAMIENTO
          }
          else if (command == "data2=0")
          {
            cont2=0;              //Configura OPERACION en MANUAL
          }
          else if (command == "data2=1")
          {
            cont2=1;              //Configura OPERACION en AUTOMATICO
          }
           else if (command == "data3=0")
          {
            cont3=0;              //Configura VENTILADOR en APAGADO
          }
          else if (command == "data3=1")
          {
            cont3=1;              //Configura VENTILADOR en ENCENDIDIO
          }
          else if (command == "data4=0")
          {
            cont4=0;              //Configura CALOVENTOR en APAGADO
          }
          else if (command == "data4=1")
          {
            cont4=1;              //Configura CALOVENTOR en ENCENDIDO
          }
          else if (command == "data5=0")
          {
            cont5=0;            //Configura el SISTEMA DE RIEGO en APAGADO
          }
          else if (command == "data5=1")
          {
            cont5=1;            //Configura el SISTEMA DE RIEGO en ENCENDIDO
          }
        }
        
        // AL RECIBIR LINEA EN BLANCO, SIRVE PAGINA AL CLIENTE
        if (c == '\n' && currentLineIsBlank)
        {
           t1 = dht1.readTemperature ();  //LEE EL VALOR DE TEMPERATURA DE ZONA 1 Y LO ALMACENA EN t1
           t2 = dht2.readTemperature ();  //LEE EL VALOR DE TEMPERATURA DE ZONA 2 Y LO ALMACENA EN t2
           h1 = dht1.readHumidity();      //LEE EL VALOR DE HUMEDAD DE ZONA 1 Y LO ALMACENA EN h2
           h2 = dht2.readHumidity();      //LEE EL VALOR DE HUMEDAD DE ZONA 2 Y LO ALMACENA EN h2
           hs = map (analogRead(sensor), 1023, 0, 0, 100); //CONVIERTE LOS VALORES EN BITES QUE ENVIA EL MODULO DEL YG-69 Y ALMACENA EN hs
                                                          
           
            if (cont1==0) // SI SE HA PRESIONADO EL PULSADOR "PARADA"
        { 
          digitalWrite(estado,LOW);     //APAGO EL INDICADOR DE ESTADO DETENIDO
          digitalWrite(operacion,LOW);  //APAGO EL INDICADOR DE OPERACION MANUAL
          digitalWrite(ventilacion,LOW);//APAGO EL INDICADOR DEL VENTILACION 
          digitalWrite(calefaccion,LOW);//APAGO EL INDICADOR DEL CALEFACCION 
          digitalWrite(riego,LOW);      //APAGO EL INDICADOR DEL SISTEMA DE RIEGO
           cont1=0;
           cont2=0; 
           cont3=0; 
           cont4=0; 
           cont5=0;
        }
        
          else if(cont1==1) //SI SE HA PRESIONADO EL BOTON DE "MARCHA"
           
          { digitalWrite(estado,HIGH); //ENCIENDO INDICADOR DE MARCHA
            
            
            if (cont2==1) // SI SE A PRESIONADO EL BOTON "AUTOMATICO"
           {
            digitalWrite(operacion,HIGH); //ENCIENDO EL INDICADOR DE FUNCIONAMIENTO AUTOMATICO
       
                if((t1<=15)&&(t2<=15))  //SI EN LA ZONA 1 Y 2 LA TEMPERATURA ES MENOR A 16ºC 
                {
                  digitalWrite(ventilacion,LOW);
                }
                
                else if ((t1>=20)||(t2>=20))//SI LA TEMP EN LA ZONA 1 O 2 ES MAYOR IGUAL A 20°C
                {
                  digitalWrite(ventilacion,HIGH);  
                }
                
                if((t1>=14)&&(t2>=14)) //SI EN LA ZONA 1 Y 2 LA TEMPERATURA ES MAYOR A 14ºC
                {
                   digitalWrite(calefaccion,LOW); 
                }
                else if ((t1<=9)||(t2<=9)) //SI LA TEMP EN LA ZONA 1 O 2 ES MENOR O IGUAL A 9°c
                { 
                  digitalWrite(calefaccion,HIGH); 
                }
             
                if((hs>60))  //SI LA HUMEDAD DEL SUELO ES MAYOR O IGUAL AL 60%
                {
                  digitalWrite(riego,LOW);  
                }
        
                else if ((hs<=45)) //SI LA HUMEDAD DEL SUELO ES MENOR O IGUAL AL 45%
                {
                  digitalWrite(riego,HIGH); //ACCIONO EL SISTEMA DE RIEGO
                } 
             
          }
          
            else if(cont2==0) //SI SE HA PRESIONADO EL BOTON DE "MANUAL"
            {
              digitalWrite(operacion,LOW); //CAMBIA LA SALIDA A BAJA INDICANDO QUE ESTA EN MANUAL
           
              if ((cont3==1)) //SI SE HA PRESIONADO EL BOTON "ON" DEL VENTILACION 
              { 
                digitalWrite(ventilacion,HIGH); // ENCIENDO EL INDICADOR DE FUNCIONAMIENTO DEL VENTILACION
              }
      
              else if(cont3==0) //SI SE HA PRESIONADO EL BOTON "OFF" DEL VENTILACION 
              {
                digitalWrite(ventilacion,LOW); // APAGO EL INDICADOR DE FUNCIONAMIENTO DEL VENTILACION
              }
        
              if (cont4==1) //SI SE HA PRESIONADO EL BOTON "ON" DEL CALEFACCION
              { 
                digitalWrite(calefaccion,HIGH); //ENCIENDO EL INDICADOR DE FUNCIONAMIENTO DEL CALEFACCION
              }
        
              else if (cont4==0)//SI SE HA PRESIONADO EL BOTON "OFF" DEL CALEFACCION
              { 
                digitalWrite(calefaccion,LOW);//APAGO EL INDICADOR DE FUNCIONAMIENTO DEL CALEFACCION
              } 

              if (cont5==1)  //SI SE HA PRESIONADO EL BOTON "ON" DEL SISTEMA DE RIEGO
              {
                  digitalWrite(riego,HIGH); //ENCIENDO EL INDICADOR DE FUNCIONAMIENTO DEL RIEGO
              }

              else if (cont5==0)  //SI SE HA PRESIONADO EL BOTON "OFF" DEL SISTEMA DE RIEGO
              {
                  digitalWrite(riego,LOW);  //APAGO EL INDICADOR DE FUNCIONAMIENTO DEL RIEGO
              }
            }
        
        }
        
         // LINEAS CORRESPONDIENTES AL DISEÑO DEL WEB SERVER
          client.println(F("HTTP/1.1 200 OK\nContent-Type: text/html"));
          client.println();
          client.println("<META HTTP-EQUIV='REFRESH' CONTENT='3'>");
          client.println(F("<html>\n<head>\n<title>INVERNADERO</title>\n</head>\n<body>"));
          client.println("<h1><center>CONTROL DE INVERNADERO</center></h1>");
          client.println(F("<div style='text-align:center;'>"));

          //SECCION ESTADO DEL SISTEMA
          client.print(F("<h2>Estado del sistema ")); // TITULO DE LA SECCION
          client.println(digitalRead(estado) == LOW ? "<h3>DETENIDO" : "<h3>FUNCIONANDO");//MUESTRO EL ESTADO DEL PIN DIGITAL ESTADO, SI ESTA EN BAJO SE MUESTRA EL CARTEL DETENIDO SINO, FUNCIONANDO
          client.println(F("<br/>"));
          client.println(F("<button onClick=location.href='./?data1=1'>MARCHA</button>")); //POSICIONAMOS Y DEFINIMOS EL BOTON MARCHA EN LA PAGINA Y LE ASOCIAMOS AL COMANDO data1=1
          client.println(F("<button onClick=location.href='./?data1=0'>PARADA</button>"));//POSICIONAMOS Y DEFINIMOS EL BOTON PARADA EN LA PAGINA Y LE ASOCIAMOS AL COMANDO data1=0
          client.println(F("<br/>"));
          
         //SECCION OPERACION 
          client.print(F("<h2>Operacion"));
          client.println(digitalRead(operacion) == LOW ? "<h3>MANUAL" : "<h3>AUTOMATICO");
          client.println(F("<br/>"));
          client.println(F("<button onClick=location.href='./?data2=1'>AUTOMATICO</button>"));
          client.println(F("<button onClick=location.href='./?data2=0'>MANUAL</button>"));
          client.println(F("<br/>"));
          
          //SECCION SENSADO (TEMPERATURA)
          
          client.println(F("<h2>Sensado de temperatura</h2>"));
         
          client.print("ZONA 1");
          client.print(" = ");
          client.print(t1); //MOSTRAMOS EL VALOR SENSADO PREVIAMENTE GUARDADO EN t1
          client.print(" &ordmC ");
          client.println(F("<br />"));
          
          client.print("ZONA 2");
          client.print(" = ");
          client.print(t2);//MOSTRAMOS EL VALOR SENSADO PREVIAMENTE GUARDADO EN t2
          client.print(" &ordmC ");
          client.println("<br />");
 
          //SECCION SENSADO (HUMEDAD)
         client.println(F("<h2>Sensado de humedad</h2>"));
         
         client.print("ZONA 1");
         client.print(" = ");
         client.print(h1);//MOSTRAMOS EL VALOR SENSADO DE HUMEDAD RELATIVA PREVIAMENTE GUARDADO EN h1 
         client.print(" % ");
         client.println(F("<br />"));
         
         client.print("ZONA 2");
         client.print(" = ");
         client.print(h2);//MOSTRAMOS EL VALOR SENSADO DE HUMEDAD RELATIVA PREVIAMENTE GUARDADO EN h2 
         client.print(" % ");
         client.println(F("<br />"));

         client.print("SUELO");
         client.print(" = ");
         client.print(hs);//MOSTRAMOS EL VALOR SENSADO DE HUMEDAD RELATIVA DEL SUELO PREVIAMENTE GUARDADO EN h2 
         client.print(" % ");
         client.println(F("<br />"));
       
        // SECCION DE ESTADO DE LA INSTALACION  
        client.println(F("<h2>Estado de la instalacion</h2>"));
    
        client.print(F("Ventilacion ="));
        client.print(digitalRead(ventilacion) == LOW ? "APAGADO" : "ENCENDIDO");
        client.println(F("<br/>"));
        client.println(F("<button onClick=location.href='./?data3=0'>OFF</button>"));
        client.println(F("<button onClick=location.href='./?data3=1'>ON</button>"));
        client.println(F("<br/><br/>"));
 
        client.print(F("Calefaccion = "));
        client.println(digitalRead(calefaccion) == LOW ?  "APAGADO" : "ENCENDIDO");
        client.println(F("<br/>"));
        client.println(F("<button onClick=location.href='./?data4=0'>OFF</button>"));
        client.println(F("<button onClick=location.href='./?data4=1'>ON</button>"));
        client.println(F("<br/><br/>"));

        client.print(F("Sistema de riego ="));
        client.println(digitalRead(riego) == LOW ?  "APAGADO" : "ENCENDIDO");
        client.println(F("<br/>"));
        client.println(F("<button onClick=location.href='./?data5=0'>OFF</button>"));
        client.println(F("<button onClick=location.href='./?data5=1'>ON</button>"));
        client.println(F("<br/>"));
      
        client.println(F("<a href='http://190.138.87.43'>Refrescar</a>"));
        client.println(F("</div>\n</body></html>"));

        
        break;
        }
        if (c == '\n') 
        {
          currentLineIsBlank = true;
        }
        else if (c != '\r') 
        {
          currentLineIsBlank = false;
        }
      }
    }
 
    delay(1);
    client.stop(); //DESCONECTAMOS EL SERVIDOR
  }
}
