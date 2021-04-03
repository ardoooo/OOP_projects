from tkinter import *
from tkinter import ttk
from algorithms import *


class StartFrame(ttk.Frame):
    global start_date
    global end_date
    global domains

    def __init__(self):
        ttk.Frame.__init__(self)

        self.options_frame = ttk.Frame(self)
        self.options_frame['borderwidth'] = 2
        self.options_frame['relief'] = 'sunken'
        self.options_frame.pack(side='left', fill=Y)

        self.label_start_date = ttk.Label(self.options_frame, text='Введите начальную дату')
        self.label_start_date.pack(side='top')
        self.entry_start_date = Entry(self.options_frame, textvariable=start_date)
        self.entry_start_date.pack(side='top')
        self.label_end_date = ttk.Label(self.options_frame, text='Введите конечную дату')
        self.label_end_date.pack(side='top')
        self.entry_end_date = Entry(self.options_frame, textvariable=end_date)
        self.entry_end_date.pack(side='top')

        self.input_frame = ttk.Frame(self.options_frame)
        self.label = ttk.Label(self.input_frame, text='Введите ссылки на сообщества')
        self.label.pack(side='top')

        self.list_entries = [Entry(self.input_frame, textvariable=domains[i]) for i in range(10)]
        self.help_frame = ttk.Frame(self.options_frame)
        self.button_start = Button(self.options_frame, text='начать анализ', command=start_analytics)
        self.count_input_entry = 1
        self.button_add = Button(self.help_frame, text='+', command=add_entry)
        self.button_delete = Button(self.help_frame, text='-', command=delete_entry)

        self.help_frame.pack(side='right', fill=Y)
        self.input_frame.pack(side='top', fill=Y)
        self.button_add.pack(anchor=NE)
        self.button_delete.pack()
        self.button_start.pack(expand=1, anchor=SE)
        self.list_entries[0].pack()

        # блок с описанием функционала

        self.description_frame = ttk.Frame(self)
        self.label1 = ttk.Label(self.description_frame, text='Справка', font='18')
        self.text = Text(self.description_frame, wrap=WORD)
        self.text.insert(0.0, 'Эта программа позволяет вам получить статистику по лайкам, репостам, \n'
                              'комментариям ПУБЛИЧНЫХ аккаунтов или сообществ в ВКонтакте. \n'
                              'Необходимо выбрать промежуток времени, за который вы хотиде получить статистику'
                              '\nДату нужно вводить в формате ДД.ММ.ГГ'
                              '\nС помощью кнопок "+" и "-" можно добавлять(удалять) поля для ввода ссылок'
                              '\nВ поля для ссылок нужно вводить только сам цифровой id или короткое название'
                              '\nНапример, из https://vk.com/drfpmi в поле нужно вставить только drfpmi')
        self.text.config(state=DISABLED)
        self.description_frame.pack(side='right', expand=1, fill=BOTH)
        self.label1.pack(side='top')
        self.text.pack(expand=1, fill=BOTH)


class CommonFrame(ttk.Frame):
    def __init__(self, name):
        ttk.Frame.__init__(self)

        self.left_frame = ttk.Frame(self)
        left_title_entry = Entry(self.left_frame, font='18', disabledforeground='Black')
        left_title_entry.insert(0, 'среднее')
        left_title_entry.configure(state=DISABLED)
        left_title_entry.pack(anchor='nw')
        self.left_frame.pack(side='left', fill=Y)

        self.right_frame = ttk.Frame(self)
        right_title_entry = Entry(self.right_frame, font='18', disabledforeground='Black')
        right_title_entry.insert(0, 'максимальное')
        right_title_entry.configure(state=DISABLED)
        right_title_entry.pack(anchor='ne')
        self.right_frame.pack(side='right', fill=Y)

        self.mid_frame = ttk.Frame(self)
        mid_title_entry = Entry(self.mid_frame, width=37, font='20', disabledforeground='Black')
        mid_title_entry.insert(0, name)
        mid_title_entry.configure(state=DISABLED)
        mid_title_entry.pack()
        self.mid_frame.pack(side='top', fill=Y)

        nav_frame = Frame(self)
        button_prev = Button(nav_frame, text='назад', command=lambda: go_frame(-1))
        button_prev.pack(side='left')
        button_next = Button(nav_frame, text='вперёд', command=lambda: go_frame(1))
        button_next.pack(side='right')
        nav_frame.pack(side='bottom', fill=X)


# функция для навигации между вкладками
def go_frame(step):
    number = notebook.index(notebook.select())
    if number + step <= len(names):
        notebook.select(number + step)


# функции для добавления(удаления) полей ввода ссылок
def add_entry():
    if start_frame.count_input_entry > 9:
        return
    start_frame.list_entries[start_frame.count_input_entry].pack()
    start_frame.count_input_entry += 1


def delete_entry():
    if start_frame.count_input_entry < 2:
        return
    start_frame.count_input_entry -= 1
    start_frame.list_entries[start_frame.count_input_entry].pack_forget()


# основная функция, срабатывает при нажатии кнопки "начать"
def start_analytics():
    global walls
    global domains
    domains = list(filter(lambda domain: domain.get() != '', domains))
    walls = [TableOfPosts(name.get(), start_date.get(), end_date.get()) for name in domains]
    filling_frame(common_frames[0], 'likes_average', 'likes_max')
    filling_frame(common_frames[1], 'reposts_average', 'reposts_max')
    filling_frame(common_frames[2], 'comments_average', 'comments_max')
    filling_frame(common_frames[3], 'views_average', 'views_max')
    notebook.select(1)


# функция, заполняющая вкладки статистикой
def filling_frame(common_frame, function_average, function_max):
    i = 0
    for name in domains:
        average_label = Label(common_frame.left_frame)
        count = getattr(walls[i], function_average)()
        average_label.configure(text=str(count), font='15')
        average_label.pack()

        max_label = Label(common_frame.right_frame)
        count = getattr(walls[i], function_max)()
        max_label.configure(text=str(count), font='15')
        max_label.pack()

        name_label = Label(common_frame.mid_frame)
        name_label.configure(text=name.get(), font='15')
        name_label.pack()

        i += 1


# основная программа
root = Tk()
notebook = ttk.Notebook(root)

start_date = StringVar()
end_date = StringVar()
domains = [StringVar() for i in range(10)]
walls = []

start_frame = StartFrame()
notebook.add(start_frame, text='начало работы')

names = []
names.append('количество лайков')
names.append('количество репостов')
names.append('количество комментариев')
names.append('количество просмотров')

common_frames = [CommonFrame(names[i]) for i in range(len(names))]
for i in range(len(names)):
    notebook.add(common_frames[i], text=names[i])


notebook.pack(expand=1, fill=BOTH)
root.mainloop()
