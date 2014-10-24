# CMake 要求バージョン
cmake_minimum_required (VERSION 2.8)

# プロジェクト名
project (LNUnicodeUtils)

#------------------------------------------------------------------------------
# Files
#------------------------------------------------------------------------------
# ヘッダファイル一覧 (いらないかも？)
set(lnuu_HEADERS
	LNUnicodeUtils.h)

# ソースファイル一覧
set(lnuu_SOURCES
	LNUnicodeUtils.cpp)

#------------------------------------------------------------------------------
# Output
#------------------------------------------------------------------------------
# スタティックライブラリとして作成
add_library(LNUnicodeUtils STATIC ${lnuu_SOURCES} ${lnuu_HEADERS})
# ↓共有ライブラリの場合はこちら
#add_library(LNUnicodeUtils SHARED ${lnuu_SOURCES} ${lnuu_HEADERS})

# 単体テストプロジェクト
include_directories(${PROJECT_SOURCE_DIR}/Source)
link_libraries(LNUnicodeUtils)
add_executable(LNUnicodeUtilsTest Test/Test.cpp)

