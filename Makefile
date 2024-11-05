NAME = webserv

CXX = c++

CXXFLAGS = -std=c++98  -g3 #-fsanitize=address #-Wall -Wextra -Werror

SRC  = srcs/main.cpp srcs/parsing/request/request.cpp \
	srcs/_Create_Server.cpp srcs/methods/_Post/post.cpp \
	srcs/response/response.cpp srcs/response/error_response.cpp srcs/parsing/config/config_parse.cpp

INC = -Iincludes

CONFIG_FILE = conf/config.conf

OBJ = $(SRC:.cpp=.o)

# .SILENT:

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(INC) $(OBJ) -o $(NAME)

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