#pragma once


#define IS_SHOW_REG_RSHIFT_A    (set.debug.showRegisters.rShiftA || set.debug.showRegisters.all)
#define IS_SHOW_REG_RSHIFT_B    (set.debug.showRegisters.rShiftB || set.debug.showRegisters.all)
#define IS_SHOW_REG_TRIGLEV     (set.debug.showRegisters.trigLev || set.debug.showRegisters.all)
#define IS_SHOW_REG_RANGE_A     (set.debug.showRegisters.range[A] || set.debug.showRegisters.all)
#define IS_SHOW_REG_RANGE_B     (set.debug.showRegisters.range[B] || set.debug.showRegisters.all)
#define IS_SHOW_REG_TRIGPARAM   (set.debug.showRegisters.trigParam || set.debug.showRegisters.all)
#define IS_SHOW_REG_PARAM_A     (set.debug.showRegisters.chanParam[A] || set.debug.showRegisters.all)
#define IS_SHOW_REG_PARAM_B     (set.debug.showRegisters.chanParam[B] || set.debug.showRegisters.all)
#define IS_SHOW_REG_TSHIFT      (set.debug.showRegisters.tShift || set.debug.showRegisters.all)
#define IS_SHOW_REG_TBASE       (set.debug.showRegisters.tBase || set.debug.showRegisters.all)


int  sDebug_GetSizeFontForConsole(void);        ///< Возвращает размер шрифта, которым нужно выводить сообщения в консоли.

