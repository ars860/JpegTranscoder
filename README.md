# Jpeg Transcoder

Более эффективный формат для сжатия jpeg

## Использование

`./encoder some_image.jpg encoded`  
`./decoder encoded decoded_image.jpg`

## Сделано:

1. Украден jpeg
   decoder [отсюда](https://koushtav.me/jpeg/tutorial/c++/decoder/2019/03/02/lets-write-a-simple-jpeg-library-part-2/)
2. К нему добавлена поддержка произвольного числа компонент внутри MCU, в частности сабсампилинга 2x2, исправлены
   небольшие баги
3. Написан jpeg encoder, складывающий зигзаги обратно в jpeg формате
4. В Хаффмане реализована подержка произвольного размера для н-граммы
5. Реализовано нумерационное кодирование
6. Реализован алгоритм кодирования в более эффектиынй формат

## Алгоритм более эффективного кодирования:

Делаем все как в jpeg-е, кроме Хаффмана, собираем все rle коэффициенты.  
Header сохраняем без изменений.  
Кодируем их вместе (не поблочно) более эффективным образом:

* Нумерационное кодирование DC и AC коэффициентов вместе
* Кодирование Хаффманом H(XX), DC и AC коэффициентов по отдельности

Если обоими способами уменьшить изображение не получилось, выдаем исходное изображение

## Результаты:

### jpeg30

| File             | Algo    | Encoded_size | Initial_size | Win       |
|------------------|---------|--------------|--------------|-----------|
| airplane30.jpg   | Huffman | 18478        | 19207        | 0.0379549 |
| arctichare30.jpg | Huffman | 11206        | 12080        | 0.072351  |
| baboon30.jpg     | Huffman | 35158        | 36113        | 0.0264448 |
| cat30.jpg        | Huffman | 35493        | 35902        | 0.0113921 |
| fruits30.jpg     | Huffman | 16984        | 17646        | 0.0375156 |
| frymire30.jpg    | Huffman | 191943       | 200860       | 0.0443941 |
| girl30.jpg       | Huffman | 22685        | 24310        | 0.0668449 |
| lena30.jpg       | Huffman | 17077        | 17578        | 0.0285015 |
| monarch30.jpg    | Huffman | 27708        | 28501        | 0.0278236 |
| peppers30.jpg    | Huffman | 18200        | 18746        | 0.0291262 |
| pool30.jpg       | Numeric | 6436         | 7467         | 0.138074  |
| sails30.jpg      | Huffman | 43926        | 45512        | 0.034848  |
| serrano30.jpg    | Huffman | 56040        | 58288        | 0.0385671 |
| tulips30.jpg     | Huffman | 36733        | 37972        | 0.0326293 |
| watch30.jpg      | Huffman | 43233        | 46741        | 0.0750519 |

Mean win: 4.68%

### jpeg80

| File             | Algo    | Encoded_size | Initial_size | Win        |
|------------------|---------|--------------|--------------|------------|
| airplane80.jpg   | Huffman | 43637        | 44079        | 0.0100275  |
| arctichare80.jpg | Huffman | 26506        | 26569        | 0.00237118 |
| baboon80.jpg     | Huffman | 86379        | 88465        | 0.0235799  |
| cat80.jpg        | Huffman | 80141        | 81974        | 0.0223607  |
| fruits80.jpg     | Huffman | 44758        | 45303        | 0.0120301  |
| frymire80.jpg    | Huffman | 417637       | 440857       | 0.0526701  |
| girl80.jpg       | Huffman | 58766        | 59979        | 0.0202237  |
| lena80.jpg       | Huffman | 43093        | 43872        | 0.0177562  |
| monarch80.jpg    | Huffman | 62535        | 64055        | 0.0237296  |
| peppers80.jpg    | Huffman | 46753        | 47929        | 0.0245363  |
| pool80.jpg       | Numeric | 14227        | 14492        | 0.018286   |
| sails80.jpg      | Huffman | 103543       | 105830       | 0.0216101  |
| serrano80.jpg    | Huffman | 131803       | 138167       | 0.0460602  |
| tulips80.jpg     | Huffman | 83206        | 85764        | 0.029826   |
| watch80.jpg      | Huffman | 98961        | 101074       | 0.0209055  |

Mean win: 2.31%