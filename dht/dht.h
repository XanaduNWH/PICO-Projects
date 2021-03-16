#define DHT_PIN     17
#define MAX_TIMINGS 85

typedef struct {
    float humidity;
    float temp_celsius;
} dht_reading;

void read_from_dht(dht_reading *result);