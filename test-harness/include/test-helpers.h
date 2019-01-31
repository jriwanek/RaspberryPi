#define MASK(var, mask) (var) &= (mask)
#define SET(var, func, pin) (var) |= ((func) << (pin))
