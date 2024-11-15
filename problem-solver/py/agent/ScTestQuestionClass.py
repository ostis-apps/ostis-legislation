class ScTestQuestionClass:
    def __init__(self, question: str, correct_answer: str, incorrect_answers: list[str]):
        self.__question = question
        self.__correct_answer = correct_answer
        self.__incorrect_answers = incorrect_answers

    @property
    def question(self) -> str:
        return self.__question

    @question.setter
    def question(self, value: str):
        self.__question = value

    @property
    def correct_answer(self) -> str:
        return self.__correct_answer

    @correct_answer.setter
    def correct_answer(self, value: str):
        self.__correct_answer = value

    @property
    def incorrect_answers(self) -> list[str]:
        return self.__incorrect_answers

    @incorrect_answers.setter
    def incorrect_answers(self, value: list[str]):
        self.__incorrect_answers = value

    def __iter__(self):
        return iter(self.__incorrect_answers)