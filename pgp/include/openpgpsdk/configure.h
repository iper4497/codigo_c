%HAVE_ALLOCA_H%
#define TIME_T_FMT	%TIME_T_FMT%

/* for silencing unused parameter warnings */
#define OPS_USED(x)	(x)=(x)

/* for tests, flag to tell gpg not to use blocking randomness */
#define GNUPG_QUICK_RANDOM "%GNUPG_QUICK_RANDOM%"

/* Avoid a bunch of #ifs */
#ifndef O_BINARY
# define O_BINARY	0
#endif
