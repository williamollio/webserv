##		HEADER

HEADER_FILES	= Connection.hpp Socket.hpp HTTPReader.hpp IOException.hpp HTTPRequest.hpp HTTPRequestGet.hpp  \
				  HTTPRequestPost.hpp HTTPRequestDelete.hpp CGIResponse.hpp CGIResponseGet.hpp CGIResponsePost.hpp \
				  CGIResponseDelete.hpp HTTPHeader.hpp HTTPException.hpp Configuration.hpp
HDR				= $(addprefix include/, $(HEADER_FILES))

##		SOURCES

SRC_DIR	= src/

MAIN_SRC= main.cpp Socket.cpp Connection.cpp HTTPReader.cpp IOException.cpp HTTPRequest.cpp HTTPRequestGet.cpp \
	 	  HTTPRequestPost.cpp HTTPRequestDelete.cpp CGIResponse.cpp CGIResponseGet.cpp CGIResponsePost.cpp \
	 	  CGIResponseDelete.cpp HTTPException.cpp HTTPHeader.cpp
CONF_SRC= Configuration.cpp
SRC		= $(addprefix src/, $(MAIN)) $(addprefix src/config/, $(CONF_SRC))

##		OBJECTS

OBJ_DIR	= obj/

CONF_OBJ= $(addprefix obj/config/, $(CONF_SRC))
MAIN_OBJ= $(addprefix obj/, $(MAIN_SRC))
OBJ		= $(MAIN_OBJ:.cpp=.o) $(CONF_OBJ:.cpp=.o)

##		COMPILER

NAME	= webserv

FLAGS	= -Wall -Wextra -g #-Werror

UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        FLAGS+= -D_NOEXCEPT="_GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW" -Wno-deprecated
    endif
    ifeq ($(UNAME_S),Darwin)
        CCFLAGS +=  -std=c++98
    endif

INC		= -Iinclude

##		RULES

all : obj/ $(NAME)

obj/ :
	mkdir obj/ obj/config

$(NAME) : $(OBJ)
	c++ $(FLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp $(HDR)
	c++ $(FLAGS) $(INC) -c $< -o $@

run : $(NAME)
	./$(NAME)

re : fclean all

clean :
	rm -f $(OBJ)

fclean : clean
	rm -f $(NAME)
	rm -rf obj/

.PHONY : all clean fclean re run