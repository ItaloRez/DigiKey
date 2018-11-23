//Código Arduino
#include <SPI.h> //biblioteca do pino serial que conecta o ethernetshield ao arduino
#include <Ethernet.h> //biblioteca que representa o protocolo tcp/ip 
#include <SoftwareSerial.h> //biblioteca bluetooth
#include <WebServer.h> //biblioteca que habilita autenticação na placa 
SoftwareSerial BT(5,6); //5 RX, 6 TX.

int a=0;
String newmac; //variavel de endereços mac
String newmac2;
char conc[13]; //concatenação de instruções enviadas pelo celular
int i; //contador da concatenação
static uint8_t mac[] = { 0xDE, 0xBD, 0xBE, 0xEF, 0xFE, 0xED }; //Define o endereco MAC
static uint8_t ip[] = { 10, 0, 0, 3 };         //Define o endereco IP
WebServer webserver("", 80); //Inicializa o servidor web na porta 80

void paginaprivada(WebServer &server, WebServer::ConnectionType type, char *url_param, bool param_complete)  
{
  /* login = admin
   * senha = digikey123
   * A biblioteca funciona apenas com criptografia Base64 então é necessario a codificação externa do login e da senha  
   * "dXNlcjp1c2Vy" é uma representação Base64 de "admin:digikey123"
   */
  if (server.checkCredentials("YWRtaW46ZGlnaWtleTEyMw=="))
  {
    server.httpSuccess();
    if (type != WebServer::HEAD)
    {
      server.println("<html>");
      server.println("<head>");
      server.println(" <title>DIGIKEY</title>");
      server.println("</head>");
      server.println("<body>");
      server.println("<h1> DigiKey - Ambientes Autonomos </h1>");
      server.println("<form method='get' action=''>  ");     //get para pegar info de variaveis
      server.println("<label> Fechadura </label> <br>");
      server.println("<input type='radio' name='acao' value='1'> On<br>"); //controle manual da fechadura
      server.println("<input type='radio' name='acao' value='0' checked> Off<br>");
      server.println("<br /> <label>Novo endereco MAC</label> <br>");
      server.println("<input type='text' name='var'/> <br> "); //campo do novo mac
      server.println("<input type='text' name='var2'/> <br> ");
      server.println("<button type='submit'>Confirmar</button>");
      server.println("</form>");
      server.println("</body>");
      server.println("</html>");

      String s="";
   
     if (param_complete == true)
      {
        s = url_param;
        newmac=s.substring(11,23);
        Serial.println(newmac);
        newmac2=s.substring(29,41);
        Serial.println(newmac2); 
        a=1;
        if (s.indexOf("acao=1")>-1)
        {
            entra();
        }
      }
  }
 }
 else
 {
   server.httpUnauthorized();
 }
}

void setup() 
{
  pinMode(3, OUTPUT);
  Serial.begin(9600); //Inicializa a comunicação entre PC e Arduino
  SPI.begin(); 
  BT.begin(38400); //Inicializa a comunicação entre arduino e modulo BT
  Ethernet.begin(mac, ip); //Inicializa o modulo ethernet
  webserver.setDefaultCommand(&paginaprivada);
  webserver.addCommand("private.html", &paginaprivada);
  webserver.begin();
}
void loop()
{
  
  char buff[64]; 
  int len = 64;  // processa requisições ao modulo 
  webserver.processConnection(buff, &len);
  
  if(BT.available()) //se o bt esta disponivel
  {
    char leit=BT.read(); //leit é o que recebemos do bt
    conc[i++]=leit; //coloca o que recebemos concatenado em conc
    
    if(leit=='\n') //comandos chegam com /n
    {
     if (a == 0) 
    {
        newmac = conc; //se pegar por aqui o newmac vai ser exatamente igual ao conc então da 0          
        a=1;           //no compareTo
    }
     int compare = newmac.compareTo(conc);  
     int compare2 = newmac2.compareTo(conc);
      //Serial.print(conc);
     //Serial.print(compare);
    
      if(compare == -13 || compare2 == -13 || compare == 0) //se o mac for igual
      {
        entra(); //função entra
       }
      else 
      {
       nentra(); //caso mac for errado cai em nentra
      }
      clean(); 
      BT.write("\r");   
    }    
  }
}

void clean() //limpar o contador da concatenação para um novo loop no codigo
{
  for (int cl=0; cl<=i; cl++)
  {
    conc[cl]=0;
  }
  i=0;
}

void entra()
{
     digitalWrite(3, HIGH); // ativa rele, abre a trava solenoide
     delay(3000);           // espera 3 segundos
     digitalWrite(3, LOW);// desativa rele, fecha a trava solenoide
}
void nentra()
{
  Serial.print("hahaha");
  delay(10000);
}

