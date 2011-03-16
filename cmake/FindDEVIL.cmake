FIND_LIBRARY(DEVIL_LIBRARY1 IL ENV LD_LIBRARY_PATH)
FIND_LIBRARY(DEVIL_LIBRARY2 ILU ENV LD_LIBRARY_PATH)
FIND_LIBRARY(DEVIL_LIBRARY3 ILUT ENV LD_LIBRARY_PATH)


FIND_LIBRARY(DEVIL_LIBRARIES 
    NAMES IL ILU ILUT 
    PATHS
    /lib
    /usr/lib
    /usr/local/lib
    $ENV{HOME}/code/install/devil/lib/.libs
)

message("DEVIL PATH: $ENV{HOME}/code/install/devil/lib")
message("DEVIL LIBS: ${DEVIL_LIBRARIES}")



#SET(DEVIL_INCLUDE_DIR /usr/include/)
SET(DEVIL_INCLUDE_DIR $ENV{HOME}/code/install/devil/include)
#SET(DEVIL_LIBRARIES ${DEVIL_LIBRARY1}
#			${DEVIL_LIBRARY2}
#			${DEVIL_LIBRARY3})


SET( DEVIL_FOUND "NO" )
IF(DEVIL_LIBRARIES )
    SET( DEVIL_FOUND "YES" )
ENDIF(DEVIL_LIBRARIES)