NAME = webserv

CXX = c++

CXXFLAGS = -std=c++98  #-Wall -Wextra -Werror #-fsanitize=address -g3

SRC  = srcs/main.cpp srcs/parsing/request/request.cpp \
		srcs/parsing/config/ServerConfig.cpp  srcs/parsing/config/config_parse.cpp

INC = -Iincludes

CONFIG_FILE = conf/config.conf

OBJ = $(SRC:.cpp=.o)

# .SILENT:

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(INC) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(OBJ)
fclean:
	rm -f $(NAME) $(OBJ)

re: fclean all clean

.PHONY: all clean fclean re

r: $(NAME)
	@clear
	@./$(NAME) $(CONFIG_FILE)

.SECONDARY: $(OBJ)