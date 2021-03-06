#include "stdafx.h"
#include "CppUnitTest.h"

#include "../GetOpt.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace UnitTest
{
   TEST_CLASS( UnitTestGetOpt )
   {
      char erroropt;
      int error_count;
      int level;
      string homePath;
      string logdir;
      string err;
      bool verbose;
      bool daemon;
      bool kill;
      void setOptions( char optionChar, GetOpt& getopt )
      {
         switch ( optionChar )
         {
            case 'H':
               homePath = getopt.get();
               break;
            case 'l':
               logdir = getopt.get();
               break;
            case 'v':
               verbose = true;
               break;
            case 'd':
               daemon = true;
               break;
            case 'k':
               kill = true;
               break;
            case 'a':
               if ( !getopt.get().empty() )
               {
                  level = std::stoi( getopt.get() );
               }
               else
               {
                  level = 1;
               }
               break;
            case '?':
            case ':':
               error_count++;
               erroropt = optionChar;
               err = getopt.error();
            case 'h':
            default:
               break;
         }
      }

   public:
      TEST_METHOD_INITIALIZE( Setup )
      {
         erroropt = '_';
         error_count = 0;
         level = 0;
         verbose = false;
         daemon = false;
         kill = false;
         homePath.clear();
         logdir.clear();
         err.clear();
      }

      TEST_METHOD( TestOpt )
      {
         char opt = 0;
         char* argv[] = { "prg.exe", "-H/user/home", "-v", "-d" };

         GetOpt getopt( _countof( argv ), argv, "H:l:vdkh" );
         while ( ( opt = getopt() ) != EOF )
         {
            setOptions( opt, getopt );
         }

         Assert::IsTrue( verbose, L"verbose is not set" );
         Assert::IsTrue( daemon, L"daemon is set" );
         Assert::IsFalse( kill, L"kill is set" );
         Assert::AreEqual( string( "/user/home" ), homePath, L"home differs" );
         Assert::IsTrue( logdir.empty(), L"logdir differs" );
         Assert::IsTrue( err.empty(), L"err differs" );
      }

      TEST_METHOD( TestOptArg )
      {
         char opt = 0;
         char* argv[] = { "prg.exe", "-H", "/user/home", "-v", "-d" };

         GetOpt getopt( _countof( argv ), argv, "H:l:vdkh" );
         while ( ( opt = getopt() ) != EOF )
         {
            setOptions( opt, getopt );
         }
         Assert::IsTrue( verbose, L"verbose is not set" );
         Assert::IsTrue( daemon, L"daemon is set" );
         Assert::IsFalse( kill, L"kill is set" );
         Assert::AreEqual( string( "/user/home" ), homePath, L"home differs" );
         Assert::IsTrue( logdir.empty(), L"logdir differs" );
         Assert::IsTrue( err.empty(), L"err differs" );
      }

      TEST_METHOD( TestOpt2Args )
      {
         char opt = 0;
         char* argv[] = { "prg.exe", "-H", "/user/home", "-l/user/log", "-d" };

         GetOpt getopt( _countof( argv ), argv, "H:l:vdkh" );
         while ( ( opt = getopt() ) != EOF )
         {
            setOptions( opt, getopt );
         }
         Assert::IsFalse( verbose, L"verbose is not set" );
         Assert::IsTrue( daemon, L"daemon is set" );
         Assert::IsFalse( kill, L"kill is set" );
         Assert::AreEqual( string( "/user/home" ), homePath, L"home differs" );
         Assert::AreEqual( string( "/user/log" ), logdir, L"log differs" );
         Assert::IsTrue( err.empty(), L"err differs" );
      }

      TEST_METHOD( TestOptMissingArg )
      {
         char opt = 0;
         char* argv[] = { "prg.exe", "-d", "-l" };

         GetOpt getopt( _countof( argv ), argv, "H:l:vdkh" );
         while ( ( opt = getopt() ) != EOF )
         {
            setOptions( opt, getopt );
         }
         Assert::IsFalse( verbose, L"verbose is not set" );
         Assert::IsTrue( daemon, L"daemon is set" );
         Assert::IsFalse( kill, L"kill is set" );
         Assert::AreEqual( string( "prg.exe: option requires argument -l\n" ), err, L"error msg differs" );
         Assert::IsTrue( logdir.empty(), L"logdir differs" );
         Assert::IsTrue( homePath.empty(), L"homePath differs" );
      }

      TEST_METHOD( TestOptUnknownArg )
      {
         char opt = 0;
         char* argv[] = { "prg.exe", "-d", "-u" };

         GetOpt getopt( _countof( argv ), argv, "H:l:vdkh" );
         while ( ( opt = getopt() ) != EOF )
         {
            setOptions( opt, getopt );
         }
         Assert::IsFalse( verbose, L"verbose is not set" );
         Assert::IsTrue( daemon, L"daemon is set" );
         Assert::IsFalse( kill, L"kill is set" );
         Assert::AreEqual( string( "prg.exe: unknown option -u\n" ), err, L"error msg differs" );
         Assert::IsTrue( logdir.empty(), L"logdir differs" );
         Assert::IsTrue( homePath.empty(), L"homePath differs" );
      }

      TEST_METHOD( TestOptMissingArgInBetween )
      {
         char opt = 0;
         char* argv[] = { "prg.exe", "-d", "-l", "-k" };

         GetOpt getopt( _countof( argv ), argv, "H:l:vdkh" );
         while ( ( opt = getopt() ) != EOF )
         {
            setOptions( opt, getopt );
         }
         Assert::IsFalse( verbose, L"verbose is not set" );
         Assert::IsTrue( daemon, L"daemon is set" );
         Assert::IsTrue( kill, L"kill is set" );
         Assert::AreEqual( string( "prg.exe: option requires argument -l\n" ), err, L"error msg differs" );
         Assert::IsTrue( logdir.empty(), L"logdir differs" );
         Assert::IsTrue( homePath.empty(), L"homePath differs" );
      }

      TEST_METHOD( TestOptStartWithArgFail )
      {
         char opt = 0;
         char* argv[] = { "prg.exe", "test", "-l/user/home", "-k" };

         GetOpt getopt( _countof( argv ), argv, "H:l:vdkh" );
         while ( ( opt = getopt() ) != EOF )
         {
            setOptions( opt, getopt );
         }
         Assert::IsFalse( verbose, L"verbose is set" );
         Assert::IsFalse( daemon, L"daemon is set" );
         Assert::IsTrue( kill, L"kill is set" );
         Assert::AreEqual( string( "/user/home" ), logdir, L"logdir differs" );
         Assert::AreEqual( string( "" ), err, L"error msg differs" );
         Assert::IsTrue( homePath.empty(), L"homePath differs" );
         auto noopts = getopt.getRemainingArguments();
         Assert::AreEqual(1, (int) noopts.size(), L"size of no opts.");
         Assert::AreEqual(std::string("test"), noopts[0]);
      }

      TEST_METHOD( TestOptStringStartWithColon )
      {
         char opt = 0;
         char* argv[] = { "prg.exe", "-l/user/home", "-k" };

         GetOpt getopt( _countof( argv ), argv, ":H:l:vdkh" );
         while ( ( opt = getopt() ) != EOF )
         {
            setOptions( opt, getopt );
         }
         Assert::IsFalse( verbose, L"verbose is set" );
         Assert::IsFalse( daemon, L"daemon is set" );
         Assert::IsTrue( kill, L"kill is set" );
         Assert::AreEqual( string( "/user/home" ), logdir, L"logdir differs" );
         Assert::AreEqual( string( "prg.exe: missing option argument in :H:l:vdkh\n" ), err, L"error msg differs" );
         Assert::AreEqual( ':', erroropt, L"opt is not :" );
         Assert::IsTrue( homePath.empty(), L"homePath differs" );
      }

      TEST_METHOD( TestOptWithNonOptionArg )
      {
         char opt = 0;
         char* argv[] = { "prg.exe", "-l/user/home", ":k" };

         GetOpt getopt( _countof( argv ), argv, "H:l:vdkh" );
         while ( ( opt = getopt() ) != EOF )
         {
            setOptions( opt, getopt );
         }
         Assert::IsFalse( verbose, L"verbose is set" );
         Assert::IsFalse( daemon, L"daemon is set" );
         Assert::IsFalse( kill, L"kill is set" );
         Assert::AreEqual( string( "/user/home" ), logdir, L"logdir differs" );
         Assert::IsTrue( err.empty(), L"error msg differs" );
         Assert::IsTrue( homePath.empty(), L"homePath differs" );
         Assert::AreEqual( ':', argv[getopt.getIndex()][0], L"First non option argument differs" );
      }

      TEST_METHOD( TestWithStopOptionArg )
      {
         char opt = 0;
         char* argv[] = { "prg.exe", "-l/user/home", "--", "noarg" };

         GetOpt getopt( _countof( argv ), argv, "H:l:vdkh" );
         while ( ( opt = getopt() ) != EOF )
         {
            setOptions( opt, getopt );
         }
         Assert::IsFalse( verbose, L"verbose is set" );
         Assert::IsFalse( daemon, L"daemon is set" );
         Assert::IsFalse( kill, L"kill is set" );
         Assert::AreEqual( string( "/user/home" ), logdir, L"logdir differs" );
         Assert::IsTrue( err.empty(), L"error msg differs" );
         Assert::IsTrue( homePath.empty(), L"homePath differs" );
         Assert::AreEqual( std::string( "noarg" ), std::string( argv[getopt.getIndex()] ), L"First non option argument differs" );
         auto noopts = getopt.getRemainingArguments();
         Assert::AreEqual(1, (int) noopts.size(), L"size of no opts.");
         Assert::AreEqual(std::string("noarg"), noopts[0]);
      }

      TEST_METHOD( TestEmptyOptString )
      {
         char opt = 0;
         char* argv[] = { "prg.exe", "-l/user/home", "-k" };

         GetOpt getopt( _countof( argv ), argv, "" );
         while ( ( opt = getopt() ) != EOF )
         {
            setOptions( opt, getopt );
         }
         Assert::IsFalse( verbose, L"verbose is set" );
         Assert::IsFalse( daemon, L"daemon is set" );
         Assert::IsFalse( kill, L"kill is set" );
         Assert::AreEqual( string( "" ), logdir, L"logdir differs" );
         Assert::AreEqual( string( "prg.exe: unknown option -k\n" ), err, L"last error msg differs" );
         Assert::AreEqual( '?', erroropt, L"opt is not _" );
         Assert::AreEqual( 2, error_count, L"error_count differs" );
         Assert::IsTrue( homePath.empty(), L"homePath differs" );
      }

      TEST_METHOD( TestEmptyOption )
      {
         char opt = 0;
         char* argv[] = { "prg.exe", "-", "l", "-k" };

         GetOpt getopt( _countof( argv ), argv, "H:l:vdkh" );
         while ( ( opt = getopt() ) != EOF )
         {
            setOptions( opt, getopt );
         }
         Assert::IsFalse( verbose, L"verbose is set" );
         Assert::IsFalse( daemon, L"daemon is set" );
         Assert::IsFalse( kill, L"kill is set" );
         Assert::AreEqual( string( "" ), logdir, L"logdir differs" );
         Assert::AreEqual( string( "" ), homePath, L"homePath differs" );
         string errtxt = string("prg.exe: unknown option -") + '\0' + "\n";
         Assert::AreEqual( errtxt, err, L"last error msg differs" );
         Assert::AreEqual( '?', erroropt, L"opt is not _" );
         Assert::AreEqual( 1, error_count, L"error_count differs" );
      }

      TEST_METHOD( TestColonOption )
      {
         char opt = 0;
         char* argv[] = { "prg.exe", "-:", "l", "-k" };

         GetOpt getopt( _countof( argv ), argv, "H:l:vdkh" );
         while ( ( opt = getopt() ) != EOF )
         {
            setOptions( opt, getopt );
         }
         Assert::IsFalse( verbose, L"verbose is set" );
         Assert::IsFalse( daemon, L"daemon is set" );
         Assert::IsFalse( kill, L"kill is set" );
         Assert::AreEqual( string( "" ), logdir, L"logdir differs" );
         Assert::AreEqual( string( "" ), homePath, L"homePath differs" );
         Assert::AreEqual( string( "prg.exe: unknown option -:\n" ), err, L"last error msg differs" );
         Assert::AreEqual( '?', erroropt, L"opt is not ?" );
         Assert::AreEqual( 1, error_count, L"error_count differs" );
      }

      TEST_METHOD( TestOptionalArgument )
      {
         char opt = 0;
         char* argv[] = { "prg.exe", "-a 7" };

         GetOpt getopt( _countof( argv ), argv, "a::" );
         while ( ( opt = getopt() ) != EOF )
         {
            setOptions( opt, getopt );
         }
         Assert::AreEqual( 7, level, L"level isn't 7" );
         Assert::AreNotEqual( string( "prg.exe: unknown option -:\n" ), err, L"last error msg differs" );
         Assert::AreNotEqual( '?', erroropt, L"opt is not ?" );
         Assert::AreEqual( 0, error_count, L"error_count differs" );
      }

      TEST_METHOD( TestOptionalArgumentNotGiven )
      {
         char opt = 0;
         char* argv[] = { "prg.exe", "-a" };

         GetOpt getopt( _countof( argv ), argv, "a::" );
         while ( ( opt = getopt() ) != EOF )
         {
            setOptions( opt, getopt );
         }
         Assert::AreEqual( 1, level, L"level isn't 1" );
         Assert::AreNotEqual( string( "prg.exe: unknown option -:\n" ), err, L"last error msg differs" );
         Assert::AreNotEqual( '?', erroropt, L"opt is not ?" );
         Assert::AreEqual( 0, error_count, L"error_count differs" );
      }

      TEST_METHOD( TestOptionalArgumentNotGivenAndNotOnly )
      {
         char opt = 0;
         char* argv[] = { "prg.exe", "-a", "-d" };

         GetOpt getopt( _countof( argv ), argv, "a::d" );
         while ( ( opt = getopt() ) != EOF )
         {
            setOptions( opt, getopt );
         }
         Assert::AreEqual( 1, level, L"level isn't 1" );
         Assert::IsTrue( daemon, L"daemon option is not set" );
         Assert::AreNotEqual( string( "prg.exe: unknown option -:\n" ), err, L"last error msg differs" );
         Assert::AreNotEqual( '?', erroropt, L"opt is not ?" );
         Assert::AreEqual( 0, error_count, L"error_count differs" );
      }

      TEST_METHOD( TestOptReset )
      {
         char opt = 0;
         char* argv[] = { "prg.exe", "-H/user/home", "-v", "-d" };

         GetOpt getopt( _countof( argv ), argv, "H:l:vdkh" );
         while ( ( opt = getopt() ) != EOF )
         {
            setOptions( opt, getopt );
         }
         Assert::IsTrue( verbose, L"verbose is not set" );
         Assert::IsTrue( daemon, L"daemon is set" );
         Assert::IsFalse( kill, L"kill is set" );
         Assert::AreEqual( string( "/user/home" ), homePath, L"home differs" );
         Assert::IsTrue( logdir.empty(), L"logdir differs" );
         Assert::IsTrue( err.empty(), L"err differs" );

         Setup();
         getopt.reset();
         while ( ( opt = getopt() ) != EOF )
         {
            setOptions( opt, getopt );
         }
         Assert::IsTrue( verbose, L"verbose is not set" );
         Assert::IsTrue( daemon, L"daemon is set" );
         Assert::IsFalse( kill, L"kill is set" );
         Assert::AreEqual( string( "/user/home" ), homePath, L"home differs" );
         Assert::IsTrue( logdir.empty(), L"logdir differs" );
         Assert::IsTrue( err.empty(), L"err differs" );

      }

   };

}