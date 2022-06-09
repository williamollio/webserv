##		HEADER

HEADER_FILES	= Connection.hpp Socket.hpp HTTPReader.hpp Configuration.hpp
HDR				= $(addprefix include/, $(HEADER_FILES))

##		SOURCES

SRC_DIR	= src/

MAIN_SRC= main.cpp Socket.cpp Connection.cpp HTTPReader.cpp
CONF_SRC= Configuration.cpp
SRC		= $(addprefix src/, $(MAIN)) $(addprefix src/config/, $(CONF_SRC))

##		OBJECTS

OBJ_DIR	= obj/

CONF_OBJ= $(addprefix obj/config/, $(CONF_SRC))
MAIN_OBJ= $(addprefix obj/, $(MAIN_SRC))
OBJ		= $(MAIN_OBJ:.cpp=.o) $(CONF_OBJ:.cpp=.o)

##		COMPILER

NAME	= webserv

FLAGS	= -Wall -Wextra -std=c++98 -g #-Werror

INC		= -Iinclude

##		RULES

all : obj/ $(NAME)

obj/ :
	mkdir obj/ obj/config

$(NAME) : $(OBJ)
	c++ $(FLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp $(HDR)
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