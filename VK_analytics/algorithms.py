import vk_api
import datetime


token = "9f8bfbdd4759757d4c366a7e5a37b7daba1b436861f96587c2caa082b84221ed9a60c1afc5843c189f0d2"

'''класс TableOfPosts, хранящий все посты по стены конкретного сообщества,
стастику о сообществе мы получаем с помощью методо класса'''


class TableOfPosts:

    def __init__(self, domain, start_date, end_date):
        self.domain = domain
        self.start_date = datetime.datetime.strptime(str(start_date), '%d.%m.%y')
        self.end_date = datetime.datetime.strptime(str(end_date), '%d.%m.%y')
        self.start_date = self.start_date.timestamp()
        self.end_date = self.end_date.timestamp()

        vk_session = vk_api.VkApi(token=token)
        tools = vk_api.VkTools(vk_session)
        self.wall = tools.get_all('wall.get', 100, {'domain': self.domain})

        # при инициализации класса сразу насчитаем некоторые параметры, для ускорения работы приложения
        self.common_count_likes = 0
        self.common_count_reposts = 0
        self.common_count_views = 0
        self.common_count_comments = 0
        self.max_count_likes = 0
        self.max_count_reposts = 0
        self.max_count_views = 0
        self.max_count_comments = 0
        for post in self.wall['items']:
            if post['date'] <= self.end_date:
                if post['date'] >= self.start_date:
                    self.common_count_likes += post['likes']['count']
                    self.max_count_likes = max(self.max_count_likes, post['likes']['count'])

                    self.common_count_reposts += post['reposts']['count']
                    self.max_count_reposts = max(self.max_count_reposts, post['reposts']['count'])

                    self.common_count_views += post['views']['count']
                    self.max_count_views = max(self.max_count_views, post['views']['count'])

                    self.common_count_comments += post['comments']['count']
                    self.max_count_comments = max(self.max_count_comments, post['comments']['count'])
                else:
                    break

    # метод, возвращающий среднее количество лайков
    def likes_average(self):
        if self.wall['count'] == 0:
            return 0
        return self.common_count_likes / self.wall['count']

    # метод, возвращающий среднее количество лайков
    def reposts_average(self):
        if self.wall['count'] == 0:
            return 0
        return self.common_count_reposts / self.wall['count']

    # метод, возвращающий среднее количество комментариев
    def comments_average(self):
        if self.wall['count'] == 0:
            return 0
        return self.common_count_comments / self.wall['count']

    # метод, возвращающий среднее количество просмотров
    def views_average(self):
        if self.wall['count'] == 0:
            return 0
        return self.common_count_views / self.wall['count']


# метод, возвращающий максимальное количество лайков
    def likes_max(self):
        return self.max_count_likes

    # метод, возвращающий максимальное количество лайков
    def reposts_max(self):
        return self.max_count_reposts

    # метод, возвращающий максимальное количество комментариев
    def comments_max(self):
        return self.max_count_comments

    # метод, возвращающий максимальное количество просмотров
    def views_max(self):
        return self.max_count_views
