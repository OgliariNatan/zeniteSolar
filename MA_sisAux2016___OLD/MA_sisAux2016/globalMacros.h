#ifndef GLOBAL_MACROS_H
#define GLOBAL_MACROS_H

#define maskByte(data, mask)            (   (data) & (mask)   )
#define getByte(from, byte, mask)       (   maskByte(from[byte], mask)   )
#define setByte(to, from, byte, mask)   to[byte] = maskByte(from, mask)
#define changeBit(to, from, byte, mask) to[byte] = maskByte((to[(byte)]), ~(mask) ) | (   ((to) ? (mask) : 0)   );

#endif /* ifndef GLOBAL_MACROS_H */

