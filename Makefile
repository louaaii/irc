SEPIA = \033[38;5;180m
BLACK = \033[0;30m
WHITE = \033[1;37m
GRAY = \033[0;37m
RESET = \033[0m
BOLD = \033[1m

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

NAME = 

SRCS = 

OBJS = $(SRCS:.cpp=.o)
       
all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

push:
	@echo "$(SEPIA)$(BOLD)Uploading data to Bunker...$(RESET)"
	@git add .
	@echo -n "$(WHITE)Enter transmission message: $(RESET)"
	@read commit_message; \
	git commit -m "$$commit_message"; \
	git push; \
	echo "$(SEPIA)Data successfully transmitted: '$$commit_message'$(RESET)"

re: fclean all 


.PHONY: all clean fclean re push
