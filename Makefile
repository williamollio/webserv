##		HEADER

HEADER_FILES	= Connection.hpp Socket.hpp HTTPReader.hpp IOException.hpp HTTPRequest.hpp CGIResponse.hpp 		\
				  CGIResponseGet.hpp CGIResponsePost.hpp CGIResponseDelete.hpp CGIResponseRedirect.hpp HTTPHeader.hpp HTTPException.hpp \
				  Configuration.hpp URI.hpp URISyntaxException.hpp Tool.hpp CGICall.hpp CGICallBuiltin.hpp 		\
				  CGIResponseError.hpp Cookie.hpp Runnable.hpp
HDR				= $(addprefix include/, $(HEADER_FILES))

##		SOURCES

SRC_DIR	= src/

MAIN_SRC = main.cpp Socket.cpp Connection.cpp HTTPReader.cpp IOException.cpp HTTPRequest.cpp CGIResponse.cpp 	 \
		   CGIResponseGet.cpp CGIResponsePost.cpp CGIResponseDelete.cpp CGIResponseRedirect.cpp HTTPException.cpp HTTPHeader.cpp URI.cpp \
		   URISyntaxException.cpp Tool.cpp CGICall.cpp CGIResponseError.cpp CGICallBuiltin.cpp Cookie.cpp
CONF_SRC= Configuration.cpp
SRC		= $(addprefix src/, $(MAIN)) $(addprefix src/config/, $(CONF_SRC))

##		OBJECTS

OBJ_DIR	= obj/

CONF_OBJ= $(addprefix obj/config/, $(CONF_SRC))
MAIN_OBJ= $(addprefix obj/, $(MAIN_SRC))
OBJ		= $(MAIN_OBJ:.cpp=.o) $(CONF_OBJ:.cpp=.o)

##		COMPILER

NAME	= webserv

FLAGS	= -Wall -Wextra -Werror -pedantic -std=c++98 -Ofast -DDEBUG=2

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
	./$(NAME) server.conf

re : fclean all

clean :
	rm -f $(OBJ)

fclean : clean
	rm -f $(NAME)
	rm -rf obj/

.PHONY : all clean fclean re run
