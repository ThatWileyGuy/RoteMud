cmake_minimum_required(VERSION 3.13)
project(RoteMud)
set(CMAKE_CXX_STANDARD 17)

find_package(Boost) # header-only for now
include_directories(${Boost_INCLUDE_DIRS})

set(SOURCE_FILES 11.cxx act_comm.cxx act_info.cxx act_move.cxx act_obj.cxx act_wiz.cxx boards.cxx bounty.cxx build.cxx changes.cxx clans.cxx
	color.cxx comm.cxx comments.cxx const.cxx db.cxx editor.cxx fight.cxx finfo.cxx force.cxx fskills.cxx functions.cxx handler.cxx
	hashstr.cxx hunter.cxx interp.cxx magic.cxx makeobjs.cxx marriage.cxx md5.cxx misc.cxx mud_comm.cxx mud_prog.cxx
	newarena.cxx pfiles.cxx planets.cxx player.cxx renumber.cxx reset.cxx save.cxx ships.cxx shops.cxx skills.cxx slay.cxx
	slicers.cxx slotm.cxx space.cxx special.cxx swskills.cxx tables.cxx tech.cxx track.cxx update.cxx)

set(HEADER_FILES bet.hxx color.hxx do_fun.hxx md5.hxx mud.hxx pfiles.hxx slay.hxx)

set(ALL_FILES ${SOURCE_FILES} ${HEADER_FILES})

add_executable(rote_mud ${ALL_FILES})