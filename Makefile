NAME	= webserv

SRC_DIR	= src/
SRC		= main.cpp Socket.cpp Connection.cpp

#HDR	=  vector.hpp iterator.hpp

OBJ_DIR	= obj/
OBJ		= $(addprefix $(OBJ_DIR), $(SRC:.cpp=.o))

FLAGS	= -Wall -Wextra -std=c++98 #-Werror

all : $(OBJ_DIR) $(NAME)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(NAME) : $(OBJ)
	c++ $(FLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp
	c++ $(FLAGS) -c $< -o $@

re : fclean all

clean :
	rm -f $(OBJ)

fclean : clean
	rm -f $(NAME)