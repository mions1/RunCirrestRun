
/**
 * Save data to file
 * @param data integer data to save
 * @param file file to save data
 * @return true if success
 * @see loadFromFile()
 */
bool saveToFile(int data,const char file[]);
/**
 * Load data from file
 * @param data integer data to save the load data
 * @param file file from load data
 * @return true if success
 * @saveToFile()
 */
bool loadFromFile(int &data,const char file[]);
