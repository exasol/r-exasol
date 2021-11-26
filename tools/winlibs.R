# Build against mingw-w64 build of openssl
VERSION <- commandArgs(TRUE)
if(!file.exists(sprintf("../windows/openssl-%s/include/openssl/ssl.h", VERSION))){
  if(getRversion() < "3.3.0") setInternet2()
  download.file(sprintf("https://github.com/rwinlib/openssl/archive/v%s.zip", VERSION),
                "lib.zip", quiet = FALSE)
  dir.create("../windows", showWarnings = TRUE)
  unzip("lib.zip", exdir = "../windows")
  unlink("lib.zip")
} else {
  message("OpenSSL already installed")
}
