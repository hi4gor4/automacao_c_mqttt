#ifndef LINUX_CREDENTIALS
#define LINUX_CREDENTIALS

/* Provide MQTT broker credentials as denoted in maqiatto.com. */
#define CLIENTID        "raspberry"
#define BROKER_ADDR     "maqiatto.com"
#define USERNAME        "hiago23rangel@gmail.com"
#define PASSWORD        "2314"

/* Provide topic as it is denoted in your topic list. 
 * For example mine is : cadominna@gmail.com/topic1
 * To add topics, see https://www.maqiatto.com/configure
 */
#define TOPIC    "hiago23rangel@gmail.com/pbl2"

#define TOPIC_R_L1 "hiago23rangel@gmail.com/r_estadol1" // Raspberry faz a publicação do estado da lampada 1
#define TOPIC_S_L1 "hiago23rangel@gmail.com/s_estadol1" // Raspberry faz o subscribe caso exista a alterção estado do site para a lampada1
#define TOPIC_R_L2 "hiago23rangel@gmail.com/r_estadol2" //Raspberry faz a publicação do estado da lampada 2
#define TOPIC_S_L2 "hiago23rangel@gmail.com/s_estadol2" // Raspberry faz o subscribe caso exista a alterção estado do site para a lampada2



#endif /* LINUX_CREDENTIALS */
