# Simple-Rotor
Um projeto para construir, da forma mais simples possível,  um rotor com azimute e elevação para rastrear satélites.

## INTRODUÇÃO
[documento em construção]

Este documento  descreve a parte mecânica, o hardware eletrônico e os códigos utilizados para implementar um rotor de azimute e elevação para rastreamento de satélites de radioamadorismo.

O rotor é controlado por software, a partir de coordenadas de azimute e elevação enviadas, via serial, por um computador rodando um software de predição orbital, oferecendo precisão suficiente para o rastreio de satélites transmitindo nas bandas de VHF e UHF.

Se buscou, no desenvolvimento dos componentes mecânicos e elétricos, a maior simplicidade possível, para que o rotor pudesse ser construído sem o uso de ferramentas especializadas e  sem a necessidade de peças impressas em 3D.

A maior parte dos componentes mecânicos e elétricos do projeto podem ser obtidas em sucatas de aparelhos eletro-eletrônicos. O projeto busca ser  flexível o suficiente para ser implementado em diferentes configurações, a depender dos materiais e peças obtidas pelo interessado na construção.


## Motores DC:

Foi feita a opção pelo uso de motores DC no projeto, devido a facilidade de obtenção, simplicidade de controle via software, e possibilidade de "ampliação de escala" de forma simples. Os motores para o projeto podem ser obtidos de fontes diversas (sucata!), incluindo brinquedos, ferramentas e eletrodomésticos a bateria, Impressoras a jato de tinta, componentes automotivos,  dentre outros. A dimensão dos motores necessários é dependente da massa total dos arranjos de antenas que se pretende mover. Antenas "leves" (conjunto menor que 2 ou 3 kg) podem ser movidas por motores com tensão nominal de funcionamento na faixa dos 5 a 12 volts.

Os motores podem ser obtidos já com uma caixa de redução associada, ou  as caixas de redução podem ser obtidas em sucatas de eletrodomésticos, brinquedos e ferramentas elétricas e adaptadas ao motor. Recomenda-se o uso de  motores e caixas de redução que forneçam, para os eixos do rotor,  uma rotação final menor ou igual a  5 RPM. 

O controle dos motores é realizado por meio de uma interface tipo “Ponte H”, que é facilmente encontrada em lojas online de eletrônica e robótica, tanto no Brasil como no Exterior.

## Feedback de posição:

A informação da posição de apontamento do rotor, fundamental para o funcionamento “automático” é obtida por potenciometros  conectados aos eixos do rotor. No entanto, outras formas de feedback da posição podem ser implementadas, com pequenos ajustes no código. A informação sobre a posição do Motor é apresentada em um visor Oled na caixa interface de controle.

## Construção mecânica:

A construção mecânica do rotor pode ser desenvolvida de diversas formas, levando-se em consideração as dimensões e pesos das antenas utilizadas, e os motores e caixas de redução obtidos. Os protótipos apresentados aqui foram desenvolvidos para apontar antenas direcionais caseiras de pequeno porte, e consistem em  caixas de madeira, abrigando os motores DC, potenciômetros de feedback de posição e engrenagens e/ou polias e correias utilizadas para transmissão de torque entre os componentes mecânicos.

O protótipo 01 é um rotor apenas de azimute, usando a caixa de redução de um expremedor de laranjas e um pequeno motor DC com redução. Foi  construído inicialmente para testar os códigos de controle. O Protótipo 02 (em construção em 20250124) manteve a construção de madeira, mas já utiliza motores já adquiridos com a redução adequada.  


## Interface de controle:
O controle do Motor e comunicação com o computador é gerenciada por meio de uma placa de desenvolvimento tipo Arduino. A escolha da placa não é crítica, desde que apresente pelo menos 2 entradas analógicas e saídas tipo PWM para controle de velocidade do rotor.
A caixa da interface de controle contém as portas de conexão para o rotor (energia e sensores), conector de entrada de energia (12v dc) e entrada USB para ca conexão com o computador de controle. A caixa de interface contém ainda um visor OLED para apresentar a medida da posição do rotor, conforme lida pelo Arduino.

## Softwares de controle:

O controle do rotor é realizado por meio de uma ponte H, comandada por uma placa de desenvolvimento tipo Arduino. A aplicação "RotorSerialPointPIDOled.ino", desenvolvida em C++, recebe uma informação de azimute e elevação via porta serial (USB), calcula o erro em relação a posição corrente do rotor, e atua uma função  de correção (velocidade, via PWM) para direcionar o rotor para o azimute e elevação recebidos. 

A aplicação "RotorPotCall" é utilizada na calibração inicial de cada protótipo de rotor, para obtenção dos valores de leitura analógicas dos potenciômetros de controle para os valores de -180 e + 180 do rotor, valores estes que precisam ser inseridos na aplicação de controle.

Uma aplicação em Phyton, "GPreader", rodando no computador de controle,  é utilizada para: 
+ Estabelecer uma conexão TCPIP com o software de predição GPredict (https://github.com/csete/gpredict), 
+ Receber  dados de apontamento (azimute e elevação) para um satélite selecionado, utilizando uma implementação em Python de parte da bibliteca Hamblib/ROTCL.
+ Obter, a partir dos primeiros dados de azimute recebidos pela aplicação, o quadrante de origem e o sentido da trajetória do satélite
+ Definir a posição inicial para partida do rotor
+ Enviar os dados de azimute e elevação via porta serial




