/*-----------------------------------------------------------------------------*/
/** @file    lcd.c                                                             */
/** @brief   The lcd system for the robot                                      */
/*-----------------------------------------------------------------------------*/

#include "lcd.h"

#include <math.h>
#include <stdlib.h>

// storage for lcd
static lcd_t lcd;

// global holds the user parameters
user_param *userp;

// magic number used for flash storage
#define MAGIC_NUMBER 13

// working area for lcd task
static WORKING_AREA(waLcd, 512);

static Thread	*lcdThreadPointer = NULL;
static long		lcdThreadDeadTimer = 0;

// private functions
static msg_t	lcdThread(void *arg);
static void		lcdRead(void);
static void		lcdWrite(void);

/*-----------------------------------------------------------------------------*/
/** @brief      Get pointer to lcd structure - not used locally                */
/** @return     A lcd_t pointer                                                */
/*-----------------------------------------------------------------------------*/
lcd_t *
lcdGetPtr(void)
{
	return (&lcd);
}

/*-----------------------------------------------------------------------------*/
/** @brief      Assign LCD display to the lcd system.                          */
/** @param[in]  display The display to use for the lcd system                  */
/*-----------------------------------------------------------------------------*/
void
lcdSetup(uint8_t display)
{
	lcd.display = display;
	lcd.mode = kLcdModeSetup;
	// read user parameters
	userp = vexFlashUserParamRead();
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Initialize the lcd system.                                     */
/*-----------------------------------------------------------------------------*/
void
lcdInit(void)
{
	lcd.mode = kLcdMode0;
	if (userp->data[0] == MAGIC_NUMBER && userp->data[1] < kLcdModeNumber) {
		lcd.mode = (kLcdModeType) userp->data[1];
	} else {
		userp->data[0] = 0;
		userp->data[1] = 0;
	}
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Start the lcd system thread                                    */
/*-----------------------------------------------------------------------------*/
void
lcdStart(void)
{
	if ((chTimeNow() - lcdThreadDeadTimer) > 300) {
		lcdThreadPointer = NULL;
	}
	if (lcdThreadPointer != NULL) {
		return;
	}
	lcdThreadPointer = chThdCreateStatic(waLcd, sizeof(waLcd), NORMALPRIO - 1, lcdThread, NULL);
	return;
}

/*-----------------------------------------------------------------------------*/
/** @brief      The lcd system thread                                          */
/** @param[in]  arg Unused                                                     */
/** @return     (msg_t) 0                                                      */
/*-----------------------------------------------------------------------------*/
static msg_t
lcdThread(void *arg)
{
	// Unused
	(void) arg;

	// Register the task
	vexTaskRegister("lcd");

	vexLcdBacklight( lcd.display, 1);
	vexLcdClearLine( lcd.display, VEX_LCD_LINE_1 );
	vexLcdClearLine( lcd.display, VEX_LCD_LINE_2 );

	while (!chThdShouldTerminate()) {
		lcdThreadDeadTimer = chTimeNow();
		lcdRead();
		lcdWrite();
		// Don't hog cpu
		vexSleep(25);
	}

	lcdThreadPointer = NULL;
	lcdThreadDeadTimer = 0;

	return ((msg_t) 0);
}

kLcdModeType
lcdGetMode(void)
{
	return lcd.mode;
}

static void
lcdRead(void)
{
	lcd.buttons = vexLcdButtonGet( lcd.display );
	if ((lcd.buttons == kLcdButtonRight) || (lcd.buttons == kLcdButtonLeft)) {
		if (lcd.buttons == kLcdButtonRight) {
			lcd.mode++;
			if (lcd.mode >= kLcdModeNumber) {
				lcd.mode = kLcdMode0;
			}
		} else if (lcd.buttons == kLcdButtonLeft) {
			lcd.mode--;
			if (lcd.mode < kLcdMode0) {
				lcd.mode = (kLcdModeNumber - 1);
			}
		}

		vexLcdClearLine( lcd.display, VEX_LCD_LINE_1 );
		vexLcdClearLine( lcd.display, VEX_LCD_LINE_2 );

		switch (lcd.mode) {
			case kLcdMode0:
				vexLcdSet( lcd.display, VEX_LCD_LINE_1, "M0"			   );
				break;
			case kLcdMode1:
				vexLcdSet( lcd.display, VEX_LCD_LINE_1, "M1"			   );
				break;
			case kLcdMode2:
				vexLcdSet( lcd.display, VEX_LCD_LINE_1, "M2"			   );
				break;
			case kLcdMode3:
				vexLcdSet( lcd.display, VEX_LCD_LINE_1, "M3"			   );
				break;
			case kLcdMode4:
				vexLcdSet( lcd.display, VEX_LCD_LINE_1, "M4"			   );
				break;
			case kLcdMode5:
				vexLcdSet( lcd.display, VEX_LCD_LINE_1, "M5"			   );
				break;
			default:
				vexLcdSet( lcd.display, VEX_LCD_LINE_1, "Error           " );
				break;
		}

		do {
			lcdThreadDeadTimer = chTimeNow();
			lcd.buttons = vexLcdButtonGet( lcd.display );
			vexSleep(10);
		} while ( lcd.buttons != kLcdButtonNone );

		userp->data[0] = MAGIC_NUMBER;
		userp->data[1] = (unsigned char) lcd.mode;

		vexFlashUserParamWrite( userp );

		vexSleep(250);
	}
}

static void
lcdWrite(void)
{
	switch (lcd.mode) {
		case kLcdMode0:
			vexLcdPrintf( lcd.display, VEX_LCD_LINE_1, "%4.2fV   %8.1f", vexSpiGetMainBattery() / 1000.0, chTimeNow() / 1000.0 );
			vexLcdPrintf( lcd.display, VEX_LCD_LINE_2, "Top Secret   4sL" );
			break;
		case kLcdMode1:
			vexLcdPrintf( lcd.display, VEX_LCD_LINE_1, "%4.2fV   %8.1f", vexSpiGetMainBattery() / 1000.0, chTimeNow() / 1000.0 );
			vexLcdPrintf( lcd.display, VEX_LCD_LINE_2, "Top Secret   4sR" );
			break;
		case kLcdMode2:
			vexLcdPrintf( lcd.display, VEX_LCD_LINE_1, "%4.2fV   %8.1f", vexSpiGetMainBattery() / 1000.0, chTimeNow() / 1000.0 );
			vexLcdPrintf( lcd.display, VEX_LCD_LINE_2, "Top Secret  4scL" );
			break;
		case kLcdMode3:
			vexLcdPrintf( lcd.display, VEX_LCD_LINE_1, "%4.2fV   %8.1f", vexSpiGetMainBattery() / 1000.0, chTimeNow() / 1000.0 );
			vexLcdPrintf( lcd.display, VEX_LCD_LINE_2, "Top Secret  4scR" );
			break;
		case kLcdMode4:
			vexLcdPrintf( lcd.display, VEX_LCD_LINE_1, "%4.2fV   %8.1f", vexSpiGetMainBattery() / 1000.0, chTimeNow() / 1000.0 );
			vexLcdPrintf( lcd.display, VEX_LCD_LINE_2, "Top Secret  C S L" );
			break;
		case kLcdMode5:
			vexLcdPrintf( lcd.display, VEX_LCD_LINE_1, "%4.2fV   %8.1f", vexSpiGetMainBattery() / 1000.0, chTimeNow() / 1000.0 );
			vexLcdPrintf( lcd.display, VEX_LCD_LINE_2, "Top Secret  C S R" );
			break;
		default:
			vexLcdPrintf( lcd.display, VEX_LCD_LINE_1, "%4.2fV   %8.1f", vexSpiGetMainBattery() / 1000.0, chTimeNow() / 1000.0 );
			vexLcdPrintf( lcd.display, VEX_LCD_LINE_2, "Top Secret   ERR" );
			break;
	}
}
