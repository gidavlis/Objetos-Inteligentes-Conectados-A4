# Sistema IoT de Alerta de Precipitação 🌧️💡

Este repositório contém o firmware, especificações de hardware e documentação de comunicação para o **Sistema IoT de Alerta de Precipitação Pluviométrica**, desenvolvido como trabalho final (A4) na disciplina de **Objetos Inteligentes Conectados** da Faculdade de Computação e Informática (FCI) na **Universidade Presbiteriana Mackenzie**.

O projeto consiste em um protótipo inteligente focado na detecção em tempo real de chuva por meio de sensoriamento digital e acionamento remoto de um sinalizador visual (LED vermelho) baseado no protocolo de internet padrão **MQTT** utilizando a infraestrutura em nuvem pública do **HiveMQ Broker**.

---

## 🛠️ Descrição do Hardware

O circuito elétrico do protótipo foi projetado para operar inteiramente sob níveis de tensão segura de $3,3\text{V}$, eliminando a necessidade de reguladores de tensão externos complexos.

### Componentes Utilizados:
1. **Unidade Central (Microcontrolador):** NodeMCU ESP12 Amica (com chip SoC ESP8266 integrado e conectividade Wi-Fi nativa de $2,4\text{ GHz}$).
2. **Sensor de Entrada:** Módulo Sensor de Chuva de fenolite dupla face de sensoriamento resistivo com comparador eletrônico LM393 regulado por potenciômetro de sensibilidade.
3. **Atuador de Saída:** Diodo Emissor de Luz (LED) vermelho de $5\text{ mm}$ (indicação visual física de precipitação).
4. **Resistor de Proteção:** Resistor de filme de carbono de $220\ \Omega$ (ligado em série ao LED para limitar a corrente e proteger a GPIO do ESP8266).
5. **Suporte e Conectores:** Protoboard de 400 furos e condutores flexíveis do tipo jumper macho-macho e macho-fêmea.

### Tabela de Conexões Físicas (Pinagem):

| Pino do Periférico | Tipo de Sinal | Pino do NodeMCU | Função Operacional |
| :---: | :---: | :---: | :--- |
| **VCC (Sensor de Chuva)** | Alimentação (+3.3V) | **3V3** | Alimentação elétrica da placa de sensoriamento e comparador |
| **GND (Sensor de Chuva)** | Referência de Terra | **GND** | Referência de potencial comum |
| **DO (Sensor de Chuva)** | Entrada Digital (IN) | **D5 (GPIO14)** | Sinal de detecção (LOW = Chuva, HIGH = Placa Seca) |
| **Ânodo / Terminal Longo (LED)**| Saída Digital (OUT) | **D6 (GPIO12)** | Sinalizador (HIGH = LED Ligado, LOW = LED Desligado) |
| **Cátodo (LED)** | Referência de Terra | **GND (via 220 &Omega;)**| Proteção elétrica para o pino de saída |

---

## 📡 Arquitetura de Rede e Protocolo MQTT

O sistema utiliza a arquitetura de transporte leve **MQTT (Message Queuing Telemetry Transport)**, ideal para internet das coisas devido à alta velocidade e mínimo consumo de banda. O nó NodeMCU atua como cliente conectado ao broker público **HiveMQ** (`broker.hivemq.com`) na porta padrão `1883`.

### Tópicos e Lógica de Mensagens:
* **Tópico de Telemetria (Publicação do Sensor):** `mackenzie/gideias/chuva`
  * O NodeMCU lê o sensor de chuva a cada **4 segundos**.
  * Se chuva for detectada, publica a string `"1"`.
  * Se a placa estiver seca, publica a string `"0"`.
* **Tópico de Controle (Inscrição do Atuador):** `mackenzie/gideias/led`
  * O NodeMCU subscreve-se assincronamente a este tópico.
  * Quando recebe o payload `"1"`, aciona o LED vermelho física e imediatamente.
  * Quando recebe o payload `"0"`, desliga o LED.

> **Importante:** Em conformidade com as normas da FCI Mackenzie, esta solução utiliza estritamente o protocolo padrão aberto MQTT e dispensa o uso de soluções proprietárias fechadas como o Blynk, proporcionando alta portabilidade.

---

## 💻 Configuração do Software (IDE do Arduino)

Para reproduzir este projeto e compilar o código fonte fornecido, siga as instruções abaixo:

### 1. Requisitos Prévios
* Instale a versão mais recente da [Arduino IDE](https://www.arduino.cc/en/software).
* Certifique-se de possuir um cabo micro-USB de boa qualidade com suporte a tráfego de dados para gravação do NodeMCU.

### 2. Configurando a Placa NodeMCU
1. Abra a Arduino IDE e acesse: `Arquivo` &rarr; `Preferências`.
2. No campo **URLs Adicionais para Gerenciadores de Placas**, insira a seguinte URL para adicionar o suporte às placas ESP8266:
   ```text
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
3. Acesse `Ferramentas` &rarr; `Placa` &rarr; `Gerenciador de Placas...`.
4. Procure por `esp8266` da **ESP8266 Community** e instale a versão mais recente.
5. Selecione a placa acessando: `Ferramentas` &rarr; `Placa` &rarr; `ESP8266 Boards` &rarr; **NodeMCU 1.0 (ESP-12E Module)**.

### 3. Instalando as Bibliotecas
1. Acesse `Ferramentas` &rarr; `Gerenciar Bibliotecas...` (ou pressione `Ctrl + Shift + I`).
2. Digite **PubSubClient** no campo de busca.
3. Localize a biblioteca desenvolvida por **Nick O'Leary** e clique em **Instalar** (utilize a versão estável mais recente).

### 4. Compilando e Gravando o Firmware
1. Clone este repositório ou baixe o arquivo de código-fonte `codigo_nodemcu.ino`.
2. Abra o arquivo na Arduino IDE.
3. Localize as variáveis de configuração de Wi-Fi e edite com os dados da sua rede local:
   ```cpp
   const char* ssid = "NOME_DA_SUA_REDE_WIFI";
   const char* password = "SENHA_DO_SUA_REDE";
   ```
4. Conecte o NodeMCU ao seu computador via cabo USB.
5. Em `Ferramentas` &rarr; `Porta`, selecione a porta COM atribuída à sua placa (se não aparecer nenhuma, certifique-se de que os drivers do conversor USB-Serial da sua placa — CH340 ou CP2102 — estão instalados no seu sistema operacional).
6. Clique no botão **Carregar (Upload)** (ícone de seta para a direita) na parte superior da IDE.
7. Ao finalizar a gravação (`Done uploading`), abra o **Monitor Serial** (`Ctrl + Shift + M`), configure a velocidade de transmissão para **115200 baud** e acompanhe as conexões de Wi-Fi e MQTT ocorrendo em tempo real!

---

## 👥 Autoria e Licença

* **Desenvolvedor:** Gideias da Silva Alves
* **Curso:** Tecnologia em Análise e Desenvolvimento de Sistemas - FCI Mackenzie
* **Instituição:** Universidade Presbiteriana Mackenzie
* **Orientação:** Prof. Dr. Willian França Costa

Este projeto é disponibilizado livremente para fins educacionais e de estudo sobre Internet das Coisas e Sistemas Conectados.
