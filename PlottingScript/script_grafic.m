test_indices = 0:9;

execution_times = [0, 0.01, 0.02, 0.2, 7.2, 69, 628, 7211, 88466.6, 841557];

%8, 16, 32, ...
matrix_dimensions = 2.^(test_indices + 3);

figure;
loglog(matrix_dimensions, execution_times, '-o', 'LineWidth', 2, 'MarkerSize', 8);

grid on;
xlabel('Dimensiunea matricii(n)');
ylabel('Timp Executie(ms)');
title('Grafic');

% afisam valorile lui n pe grafic
set(gca, 'XTick', matrix_dimensions, 'XTickLabel', string(matrix_dimensions));
set(gca, 'YTick', execution_times, 'YTickLabel', string(execution_times));