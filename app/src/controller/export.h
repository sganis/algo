#pragma once
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class

// Define EXPORTED for any platform
#if defined _WIN32 || defined __CYGWIN__
  # ifdef WIN_EXPORT

// Exporting...
    #  ifdef __GNUC__
      #   define EXPORTED __attribute__((dllexport))
    #  else // ifdef __GNUC__
      #   define EXPORTED __declspec(dllexport) // Note: actually gcc seems to
                                                // also supports this syntax.
    #  endif // ifdef __GNUC__
  # else // ifdef WIN_EXPORT
    #  ifdef __GNUC__
      #   define EXPORTED __attribute__((dllimport))
    #  else // ifdef __GNUC__
      #   define EXPORTED __declspec(dllimport) // Note: actually gcc seems to
                                                // also supports this syntax.
    #  endif // ifdef __GNUC__
  # endif // ifdef WIN_EXPORT
  # define NOT_EXPORTED
#else // if defined _WIN32 || defined __CYGWIN__
  # if __GNUC__ >= 4
    #  define EXPORTED __attribute__((visibility("default")))
    #  define NOT_EXPORTED  __attribute__((visibility("hidden")))
  # else // if __GNUC__ >= 4
    #  define EXPORTED
    #  define NOT_EXPORTED
  # endif // if __GNUC__ >= 4
#endif // if defined _WIN32 || defined __CYGWIN__
