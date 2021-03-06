/*
 * app_terminal.h
 *
 *  Created on: 2012-9-21
 *      Author: YangZhiyong
 */

#ifndef APP_TERMINAL_H_
#define APP_TERMINAL_H_


#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include "transport/transport_if.h"



void        Term_Init           (struct TRANSPORT_IF* tp);


uint32_t    Term_ReadLine       (struct TRANSPORT_IF* tp,
                                 uint8_t* buffer,
                                 uint32_t buffer_length,
                                 uint32_t timeout    );


uint32_t    Term_Write          (struct TRANSPORT_IF* tp,
                                 uint8_t* const buffer,
                                 uint32_t buffer_length,
                                 uint32_t timeout   );


uint32_t    Term_WriteLine      (struct TRANSPORT_IF* tp,
                                 uint8_t* const line,
                                 uint32_t timeout   );


uint32_t    Term_WriteString    (struct TRANSPORT_IF* tp,
                                 uint8_t* string,
                                 uint32_t timeout  );



#ifdef __cplusplus
}
#endif

#endif /* APP_TERMINAL_H_ */
