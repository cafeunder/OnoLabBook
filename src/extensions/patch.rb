module ReVIEW
	class LATEXBuilder
		def table_header(id, caption)
			puts '\begin{table}[!htbp]'	# 配置を変える
			puts '\centering'
			if caption.present?
				@table_caption = true
				puts macro('reviewtablecaption', compile_inline(caption))
			end
			puts macro('label', table_label(id))
		end

		def inline_b(str)
			if str.chars[-1].bytesize == 1
				macro('textbf', escape(str))
			else
				# 最後が和文なら \kanjiskip 挿入のためこっち
				'{\bfseries ' + escape(str) + '}'
			end
		end

		def inline_fn(id)
			if @book.config["footnotetext"]
				macro("footnotemark[#{@chapter.footnote(id).number}]", "")
			else
				'\nobreak' + macro('footnote', compile_inline(@chapter.footnote(id).content.strip))
			end
		end
	end

	class HTMLBuilder
		def image_image(id, caption, metric)
			unless metric.nil?
				tmp = metric.split('=')
				if tmp[0] === 'scale'
					min = 1
					argmin = 0
					num = tmp[1].to_f
					lst = [ 0.1, 0.2, 0.25, 0.3, 0.33, 0.4, 0.5, 0.6, 0.67, 0.70, 0.75, 0.80, 0.9, 1 ]
					lst.each_with_index do | v, i |
						diff = (num - v).abs
						if diff < min
							min = diff
							argmin = i
						end
					end
					metric = 'scale=' + lst[argmin].to_s
				end
			end

			metrics = parse_metric("html", metric)
			puts %Q[<div id="#{normalize_id(id)}" class="image">]
			puts %Q[<img src="#{@chapter.image(id).path.sub(/\A\.\//, "")}" alt="#{escape_html(compile_inline(caption))}"#{metrics} />]
			image_header id, caption
			puts %Q[</div>]
		end
	end
end
