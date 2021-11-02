#define NUM_MAX_SEQUENCE_PARAM 30

struct sequenceParamStruct
{
	unsigned int frame;
	unsigned int sample;
};

typedef struct sequenceParamStruct sequenceParamStruct;


extern int numSequenceParam;

extern sequenceParamStruct sequenceParams[NUM_MAX_SEQUENCE_PARAM];

void convertPaletteIfRequired(unsigned char* lpalette);
