function str = formatRational(num, denom)

    if denom == 1
        str = sprintf('%d', num);
    else
        str = sprintf('%d/%d', num, denom);
    end