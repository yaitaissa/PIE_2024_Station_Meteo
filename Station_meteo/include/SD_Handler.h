#include "FS.h"
#include "SD.h"
#include "SPI.h"

/**
   * @fn logToSD
   * @brief Register message to file at path.
   * @param path The path to the file
   * @n     Must be a string
   * @param message The message to register
   * @n     Must be a string
   * @return None
   */
void logToSD(const char *path, const char *message);


/**
   * @fn initializeLogFile
   * @brief Initialize the file in which to log the mesurements.
   * @param path The path to the file
   * @n     Must be a string
   * @note If the file exists, the new logs will be appended to the end of it.
   * @n Otherwise, the file will be created
   * @return None
   */
void initializeLogFile(const char *path);