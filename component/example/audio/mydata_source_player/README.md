# CUSTOMER IMPLEMENTATION DATA SOURCE PLAY EXAMPLE

## Description:
**RTPlayer supports playing a customer implementation data source.
Mydata_source.c gives an example of the detailed implementation of a customized source.**

# HW Configuration
Required Components: speaker

connect the speaker to board.

# BUILD COMMAND
./build.py -a mydata_source_player -p

## Usage:
CMD
mydata_source_player

## Note:
**Because each specific source is uncertain, there may be cases such as discontinuous data acquisition during playback, uncertainty of the total size of the data source and so on.
The simulation code of these conditions is given in mydata_source.c for reference only.
Please open these configs to simulate the above conditions:**
```
/* prepare delay simulation */
//#define MDS_PREPARE_DELAY_TEST

/* source read unsmooth simulation */
//#define MDS_READ_UNSMOOTH_TEST

/* unknown length source simulation */
//#define MDS_UNKNOWN_LENGTH_TEST
```

## Result description
        the corresponding music played in the speaker.
