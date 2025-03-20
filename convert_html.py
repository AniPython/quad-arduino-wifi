def read_and_convert_html_to_single_line(file_path):
    # 读取文件内容
    with open(file_path, 'r', encoding='utf-8') as file:
        lines = file.readlines()

    # 处理每一行
    processed_lines = []
    for line in lines:
        # 删除每行开头的空白
        stripped_line = line.lstrip()

        # 跳过以 <!-- 和 // 开头的注释行
        if not stripped_line.startswith('<!--') and not stripped_line.startswith('//'):
            processed_lines.append(stripped_line)

    # 将处理后的行合并为单行字符串
    single_line_content = ''.join(processed_lines).replace('\n', '').replace('"', '\\"')

    # 添加 Arduino String 类型的开头和结尾
    arduino_string = f'String indexHtml = "{single_line_content}";'

    return arduino_string


def html_to_arduino_string(html_file_path):
    import re

    # 读取 HTML 文件内容
    with open(html_file_path, 'r', encoding='utf-8') as file:
        lines = file.readlines()

    # 删除注释行（以 <!-- 或 // 开头，可能有前置空白）
    filtered_lines = [
        line for line in lines
        if not re.match(r'^\s*(<!--|//)', line)
    ]

    # 合并成单行字符串并转为 Arduino 格式
    single_line = ''.join(filtered_lines).replace('\n', '').replace('"', '\\"')

    # 转换为 Arduino String 格式
    arduino_string = f'String indexHtml = "{single_line}";'

    return arduino_string

# 读取并转换 index.html 文件
file_path = 'index.html'
arduino_html_template = html_to_arduino_string(file_path)

# 输出结果
print(arduino_html_template)
