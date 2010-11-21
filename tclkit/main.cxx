/*
 *----------------------------------------------------------------------
 * This wrapper function around the real main is so that CMake will use
 * c++ to link this c program. 
 *----------------------------------------------------------------------
 */
 
     
extern "C" int cmain(int argc, char** argv);

int main(int argc, char** argv)
{
  return cmain(argc, argv);
}
